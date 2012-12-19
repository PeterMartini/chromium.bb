/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CCDebugBorderDrawQuad_h
#define CCDebugBorderDrawQuad_h

#include "CCDrawQuad.h"
#include "SkColor.h"
#include <wtf/PassOwnPtr.h>

namespace WebCore {

#pragma pack(push, 4)

class CCDebugBorderDrawQuad : public CCDrawQuad {
public:
    static PassOwnPtr<CCDebugBorderDrawQuad> create(const CCSharedQuadState*, const IntRect&, SkColor, int width);

    SkColor color() const { return m_color; };
    int width() const { return m_width; }

    static const CCDebugBorderDrawQuad* materialCast(const CCDrawQuad*);
private:
    CCDebugBorderDrawQuad(const CCSharedQuadState*, const IntRect&, SkColor, int width);

    SkColor m_color;
    int m_width;
};

#pragma pack(pop)

}

#endif
