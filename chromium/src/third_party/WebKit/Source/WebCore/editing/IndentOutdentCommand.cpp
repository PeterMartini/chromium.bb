/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (IndentOutdentCommandINCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "IndentOutdentCommand.h"

#include "Document.h"
#include "HTMLElement.h"
#include "HTMLNames.h"
#include "InsertLineBreakCommand.h"
#include "InsertListCommand.h"
#include "Range.h"
#include "RenderObject.h"
#include "SplitElementCommand.h"
#include "Text.h"
#include "TextIterator.h"
#include "htmlediting.h"
#include "visible_units.h"
#include <wtf/StdLibExtras.h>

namespace WebCore {

using namespace HTMLNames;

static bool isListOrIndentBlockquote(const Node* node)
{
    return node && (node->hasTagName(ulTag) || node->hasTagName(olTag) || node->hasTagName(blockquoteTag));
}

static bool isIndentBlockquote(const Node* node)
{
    return node && node->hasTagName(blockquoteTag);
}

IndentOutdentCommand::IndentOutdentCommand(Document* document, EIndentType typeOfAction, int marginInPixels, bool isBBVersion)
    : ApplyBlockElementCommand(document, blockquoteTag, "margin: 0 0 0 40px; border: none; padding: 0px;")
    , m_typeOfAction(typeOfAction)
    , m_marginInPixels(marginInPixels)
    , m_isBBVersion(isBBVersion)
{
}

bool IndentOutdentCommand::tryIndentingAsListItem(const Position& start, const Position& end)
{
    // If our selection is not inside a list, bail out.
    Node* lastNodeInSelectedParagraph = start.deprecatedNode();
    RefPtr<Element> listNode = enclosingList(lastNodeInSelectedParagraph);
    if (!listNode)
        return false;

    // Find the block that we want to indent.  If it's not a list item (e.g., a div inside a list item), we bail out.
    Element* selectedListItem = enclosingBlock(lastNodeInSelectedParagraph);

    // FIXME: we need to deal with the case where there is no li (malformed HTML)
    if (!selectedListItem->hasTagName(liTag))
        return false;
    
    // FIXME: previousElementSibling does not ignore non-rendered content like <span></span>.  Should we?
    Element* previousList = selectedListItem->previousElementSibling();
    Element* nextList = selectedListItem->nextElementSibling();

    RefPtr<Element> newList = document()->createElement(listNode->tagQName(), false);
    insertNodeBefore(newList, selectedListItem);

    moveParagraphWithClones(start, end, newList.get(), selectedListItem);

    if (canMergeLists(previousList, newList.get()))
        mergeIdenticalElements(previousList, newList);
    if (canMergeLists(newList.get(), nextList))
        mergeIdenticalElements(newList, nextList);

    return true;
}

void IndentOutdentCommand::indentIntoBlockquote(const Position& start, const Position& end, RefPtr<Element>& targetBlockquote, bool isIndentingEntireList)
{
    Node* enclosingCell = enclosingNodeOfType(start, &isTableCell);
    Node* nodeToSplitTo;
    if (enclosingCell)
        nodeToSplitTo = enclosingCell;
    else if (enclosingList(start.containerNode()) && !isIndentingEntireList)
        nodeToSplitTo = enclosingBlock(start.containerNode());
    else
        nodeToSplitTo = editableRootForPosition(start);

    if (!nodeToSplitTo)
        return;

    RefPtr<Node> nodeAfterStart = start.computeNodeAfterPosition();
    RefPtr<Node> outerBlock = (start.containerNode() == nodeToSplitTo) ? start.containerNode() : splitTreeToNode(start.containerNode(), nodeToSplitTo);

    VisiblePosition startOfContents = start;
    if (!targetBlockquote) {
        // Create a new blockquote and insert it as a child of the root editable element. We accomplish
        // this by splitting all parents of the current paragraph up to that point.
        targetBlockquote = createBlockElement();
        if (outerBlock == start.containerNode())
            insertNodeAt(targetBlockquote, start);
        else
            insertNodeBefore(targetBlockquote, outerBlock);
        startOfContents = positionInParentAfterNode(targetBlockquote.get());
    }

    moveParagraphWithClones(startOfContents, end, targetBlockquote.get(), outerBlock.get());
}

void IndentOutdentCommand::outdentParagraph()
{
    VisiblePosition visibleStartOfParagraph = startOfParagraph(endingSelection().visibleStart());
    VisiblePosition visibleEndOfParagraph = endOfParagraph(visibleStartOfParagraph);

    Node* enclosingNode = enclosingNodeOfType(visibleStartOfParagraph.deepEquivalent(), &isListOrIndentBlockquote);
    if (!enclosingNode || !enclosingNode->parentNode()->rendererIsEditable()) // We can't outdent if there is no place to go!
        return;

    if (m_isBBVersion) {
        if (isListElement(enclosingNode)) {
            Node* highestList = highestEnclosingNodeOfType(visibleStartOfParagraph.deepEquivalent(), &isListElement);
            if (enclosingNode == highestList) {
                return;
            }
        }
    }

    // Use InsertListCommand to remove the selection from the list
    if (enclosingNode->hasTagName(olTag)) {
        applyCommandToComposite(InsertListCommand::create(document(), InsertListCommand::OrderedList));
        return;        
    }
    if (enclosingNode->hasTagName(ulTag)) {
        applyCommandToComposite(InsertListCommand::create(document(), InsertListCommand::UnorderedList));
        return;
    }
    
    // The selection is inside a blockquote i.e. enclosingNode is a blockquote
    VisiblePosition positionInEnclosingBlock = VisiblePosition(firstPositionInNode(enclosingNode));
    // If the blockquote is inline, the start of the enclosing block coincides with
    // positionInEnclosingBlock.
    VisiblePosition startOfEnclosingBlock = (enclosingNode->renderer() && enclosingNode->renderer()->isInline()) ? positionInEnclosingBlock : startOfBlock(positionInEnclosingBlock);
    VisiblePosition lastPositionInEnclosingBlock = VisiblePosition(lastPositionInNode(enclosingNode));
    VisiblePosition endOfEnclosingBlock = endOfBlock(lastPositionInEnclosingBlock);
    if (visibleStartOfParagraph == startOfEnclosingBlock &&
        visibleEndOfParagraph == endOfEnclosingBlock) {
        // The blockquote doesn't contain anything outside the paragraph, so it can be totally removed.
        Node* splitPoint = enclosingNode->nextSibling();
        removeNodePreservingChildren(enclosingNode);
        // outdentRegion() assumes it is operating on the first paragraph of an enclosing blockquote, but if there are multiply nested blockquotes and we've
        // just removed one, then this assumption isn't true. By splitting the next containing blockquote after this node, we keep this assumption true
        if (splitPoint) {
            if (ContainerNode* splitPointParent = splitPoint->parentNode()) {
                if (splitPointParent->hasTagName(blockquoteTag)
                    && !splitPoint->hasTagName(blockquoteTag)
                    && splitPointParent->parentNode()->rendererIsEditable()) // We can't outdent if there is no place to go!
                    splitElement(static_cast<Element*>(splitPointParent), splitPoint);
            }
        }

        document()->updateLayoutIgnorePendingStylesheets();
        visibleStartOfParagraph = VisiblePosition(visibleStartOfParagraph.deepEquivalent());
        visibleEndOfParagraph = VisiblePosition(visibleEndOfParagraph.deepEquivalent());
        if (visibleStartOfParagraph.isNotNull() && !isStartOfParagraph(visibleStartOfParagraph))
            insertNodeAt(createBreakElement(document()), visibleStartOfParagraph.deepEquivalent());
        if (visibleEndOfParagraph.isNotNull() && !isEndOfParagraph(visibleEndOfParagraph))
            insertNodeAt(createBreakElement(document()), visibleEndOfParagraph.deepEquivalent());

        return;
    }
    Node* enclosingBlockFlow = enclosingBlock(visibleStartOfParagraph.deepEquivalent().deprecatedNode());
    RefPtr<Node> splitBlockquoteNode = enclosingNode;
    if (enclosingBlockFlow != enclosingNode)
        splitBlockquoteNode = splitTreeToNode(enclosingBlockFlow, enclosingNode, true);
    else {
        // We split the blockquote at where we start outdenting.
        Node* highestInlineNode = highestEnclosingNodeOfType(visibleStartOfParagraph.deepEquivalent(), isInline, CannotCrossEditingBoundary, enclosingBlockFlow);
        splitElement(static_cast<Element*>(enclosingNode), (highestInlineNode) ? highestInlineNode : visibleStartOfParagraph.deepEquivalent().deprecatedNode());
    }
    RefPtr<Node> placeholder = createBreakElement(document());
    insertNodeBefore(placeholder, splitBlockquoteNode);
    moveParagraph(startOfParagraph(visibleStartOfParagraph), endOfParagraph(visibleEndOfParagraph), positionBeforeNode(placeholder.get()), true);
}

// FIXME: We should merge this function with ApplyBlockElementCommand::formatSelection
void IndentOutdentCommand::outdentRegion(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    VisiblePosition endOfLastParagraph = endOfParagraph(endOfSelection);

    if (endOfParagraph(startOfSelection) == endOfLastParagraph) {
        if (m_isBBVersion) {
            setEndingSelection(endOfSelection);
        }
        outdentParagraph();
        return;
    }
    
    Position originalSelectionEnd = m_isBBVersion ? endOfSelection.deepEquivalent() : endingSelection().end();
    VisiblePosition endOfCurrentParagraph = endOfParagraph(startOfSelection);
    VisiblePosition endAfterSelection = endOfParagraph(endOfParagraph(endOfSelection).next());

    while (endOfCurrentParagraph != endAfterSelection) {
        VisiblePosition endOfNextParagraph = endOfParagraph(endOfCurrentParagraph.next());
        if (endOfCurrentParagraph == endOfLastParagraph)
            setEndingSelection(VisibleSelection(originalSelectionEnd, DOWNSTREAM));
        else
            setEndingSelection(endOfCurrentParagraph);
        
        outdentParagraph();
        
        // outdentParagraph could move more than one paragraph if the paragraph
        // is in a list item. As a result, endAfterSelection and endOfNextParagraph
        // could refer to positions no longer in the document.
        if (endAfterSelection.isNotNull() && !endAfterSelection.deepEquivalent().anchorNode()->inDocument())
            break;
            
        if (endOfNextParagraph.isNotNull() && !endOfNextParagraph.deepEquivalent().anchorNode()->inDocument()) {
            endOfCurrentParagraph = endingSelection().end();
            endOfNextParagraph = endOfParagraph(endOfCurrentParagraph.next());
        }
        endOfCurrentParagraph = endOfNextParagraph;
    }
}

void IndentOutdentCommand::formatSelection(const VisiblePosition& startOfSelection, const VisiblePosition& endOfSelection)
{
    if (m_isBBVersion) {
        if (m_typeOfAction == Indent) {
            // Special case empty unsplittable elements because there's nothing to split
            // and there's nothing to move.
            Position start = startOfSelection.deepEquivalent().downstream();
            if (isAtUnsplittableElement(start)) {
                RefPtr<Element> blockquote = createBlockElement();
                insertNodeAt(blockquote, start);
                RefPtr<Element> placeholder = createBreakElement(document());
                appendNode(placeholder, blockquote);
                setEndingSelection(VisibleSelection(positionBeforeNode(placeholder.get()), DOWNSTREAM, endingSelection().isDirectional()));
                return;
            }
        }

        formatSelectionBB(startOfSelection, endOfSelection);
        return;
    }

    if (m_typeOfAction == Indent)
        ApplyBlockElementCommand::formatSelection(startOfSelection, endOfSelection);
    else
        outdentRegion(startOfSelection, endOfSelection);
}

bool areParagraphsOnSameListItem(const VisiblePosition& first, const VisiblePosition& second)
{
	Node* firstListItem = highestEnclosingNodeOfType(first.deepEquivalent(), &isListItem);
	Node* secondListItem = highestEnclosingNodeOfType(second.deepEquivalent(), &isListItem);
	return firstListItem && firstListItem == secondListItem;
}

void IndentOutdentCommand::formatRange(const Position& start, const Position& end, const Position&, RefPtr<Element>& blockquoteForNextIndent)
{
    if (m_isBBVersion) {
        Node* highestList = highestEnclosingNodeOfType(start, &isListElement);
        if (highestList) {
            if (isFirstVisiblePositionInNode(start, highestList) && isLastVisiblePositionInNode(end, highestList)) {
                if (m_typeOfAction == Indent) {
                    indentIntoBlockquote(start, end, blockquoteForNextIndent, true);
                }
                else {
                    Node* enclosingNode = enclosingNodeOfType(start, &isIndentBlockquote);
                    if (!enclosingNode || !enclosingNode->parentNode()->rendererIsEditable()) // We can't outdent if there is no place to go!
                        return;

                    splitTreeToNode(highestList, enclosingNode, true);
                    if (highestList->nextSibling()) {
                        splitTreeToNode(highestList->nextSibling(), enclosingNode, true);
                        enclosingNode = enclosingNodeOfType(start, &isIndentBlockquote);
                    }

                    removeNodePreservingChildren(enclosingNode);
                    document()->updateLayoutIgnorePendingStylesheets();
                }
                return;
            }
        }

        VisiblePosition startOfCurrentParagraph = startOfParagraph(start);
        VisiblePosition endOfCurrentParagraph = endOfParagraph(start);
        VisiblePosition endOfLastParagraph = endOfParagraph(end);

        while (true) {
            VisiblePosition startOfNextParagraph = endOfCurrentParagraph.next();

            if (m_typeOfAction == Outdent) {
				// If startOfCurrentParagraph and startOfNextParagraph are on the same
				// list item, outdentRegion will still outdent the entire list item,
				// which causes startOfNextParagraph to point to a removed node.
				// We need to keep moving to the end of the next paragraph until the
				// current paragraph and the next paragraph are not under the same list item.
				while (endOfCurrentParagraph != endOfLastParagraph && areParagraphsOnSameListItem(startOfCurrentParagraph, startOfNextParagraph)) {
					endOfCurrentParagraph = endOfParagraph(startOfNextParagraph);
					startOfNextParagraph = endOfCurrentParagraph.next();
				}
                outdentRegion(startOfCurrentParagraph, endOfCurrentParagraph);
            }
            else {
                if (tryIndentingAsListItem(startOfCurrentParagraph.deepEquivalent(), endOfCurrentParagraph.deepEquivalent()))
                    blockquoteForNextIndent = 0;
                else
                    indentIntoBlockquote(startOfCurrentParagraph.deepEquivalent(), endOfCurrentParagraph.deepEquivalent(), blockquoteForNextIndent);
            }

            if (endOfCurrentParagraph == endOfLastParagraph) {
                break;
            }
            startOfCurrentParagraph = startOfNextParagraph;
            endOfCurrentParagraph = endOfParagraph(startOfCurrentParagraph);
        }

        return;
    }

    if (tryIndentingAsListItem(start, end))
        blockquoteForNextIndent = 0;
    else
        indentIntoBlockquote(start, end, blockquoteForNextIndent);
}

}
