// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ppapi/proxy/serialized_structs.h"

#include "base/pickle.h"
#include "build/build_config.h"
#include "ppapi/c/dev/ppb_font_dev.h"
#include "ppapi/c/pp_file_info.h"
#include "ppapi/c/pp_rect.h"
#include "ppapi/c/trusted/ppb_browser_font_trusted.h"
#include "ppapi/shared_impl/var.h"

namespace ppapi {
namespace proxy {

SerializedFontDescription::SerializedFontDescription()
    : face(),
      family(0),
      size(0),
      weight(0),
      italic(PP_FALSE),
      small_caps(PP_FALSE),
      letter_spacing(0),
      word_spacing(0) {
}

SerializedFontDescription::~SerializedFontDescription() {}

void SerializedFontDescription::SetFromPPFontDescription(
    const PP_FontDescription_Dev& desc) {
  StringVar* string_var = StringVar::FromPPVar(desc.face);
  face = string_var ? string_var->value() : std::string();

  family = desc.family;
  size = desc.size;
  weight = desc.weight;
  italic = desc.italic;
  small_caps = desc.small_caps;
  letter_spacing = desc.letter_spacing;
  word_spacing = desc.word_spacing;
}

void SerializedFontDescription::SetFromPPBrowserFontDescription(
    const PP_BrowserFont_Trusted_Description& desc) {
  StringVar* string_var = StringVar::FromPPVar(desc.face);
  face = string_var ? string_var->value() : std::string();

  family = desc.family;
  size = desc.size;
  weight = desc.weight;
  italic = desc.italic;
  small_caps = desc.small_caps;
  letter_spacing = desc.letter_spacing;
  word_spacing = desc.word_spacing;
}

void SerializedFontDescription::SetToPPFontDescription(
    PP_FontDescription_Dev* desc) const {
  desc->face = StringVar::StringToPPVar(face);
  desc->family = static_cast<PP_FontFamily_Dev>(family);
  desc->size = size;
  desc->weight = static_cast<PP_FontWeight_Dev>(weight);
  desc->italic = italic;
  desc->small_caps = small_caps;
  desc->letter_spacing = letter_spacing;
  desc->word_spacing = word_spacing;
}

void SerializedFontDescription::SetToPPBrowserFontDescription(
    PP_BrowserFont_Trusted_Description* desc) const {
  desc->face = StringVar::StringToPPVar(face);
  desc->family = static_cast<PP_BrowserFont_Trusted_Family>(family);
  desc->size = size;
  desc->weight = static_cast<PP_BrowserFont_Trusted_Weight>(weight);
  desc->italic = italic;
  desc->small_caps = small_caps;
  desc->letter_spacing = letter_spacing;
  desc->word_spacing = word_spacing;
}

PPBFlash_DrawGlyphs_Params::PPBFlash_DrawGlyphs_Params()
    : instance(0),
      font_desc(),
      color(0) {
  clip.point.x = 0;
  clip.point.y = 0;
  clip.size.height = 0;
  clip.size.width = 0;
  position.x = 0;
  position.y = 0;
  allow_subpixel_aa = PP_FALSE;
}

PPBFlash_DrawGlyphs_Params::~PPBFlash_DrawGlyphs_Params() {}

}  // namespace proxy
}  // namespace ppapi
