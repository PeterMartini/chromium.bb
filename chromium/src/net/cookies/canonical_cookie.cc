// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Portions of this code based on Mozilla:
//   (netwerk/cookie/src/nsCookieService.cpp)
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2003
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Daniel Witte (dwitte@stanford.edu)
 *   Michiel van Leeuwen (mvl@exedo.nl)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "net/cookies/canonical_cookie.h"

#include "base/basictypes.h"
#include "base/format_macros.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "googleurl/src/gurl.h"
#include "googleurl/src/url_canon.h"
#include "net/cookies/cookie_util.h"
#include "net/cookies/parsed_cookie.h"

using base::Time;
using base::TimeDelta;

namespace net {

namespace {

// Determine the cookie domain to use for setting the specified cookie.
bool GetCookieDomain(const GURL& url,
                     const ParsedCookie& pc,
                     std::string* result) {
  std::string domain_string;
  if (pc.HasDomain())
    domain_string = pc.Domain();
  return cookie_util::GetCookieDomainWithString(url, domain_string, result);
}

std::string CanonPathWithString(const GURL& url,
                                const std::string& path_string) {
  // The RFC says the path should be a prefix of the current URL path.
  // However, Mozilla allows you to set any path for compatibility with
  // broken websites.  We unfortunately will mimic this behavior.  We try
  // to be generous and accept cookies with an invalid path attribute, and
  // default the path to something reasonable.

  // The path was supplied in the cookie, we'll take it.
  if (!path_string.empty() && path_string[0] == '/')
    return path_string;

  // The path was not supplied in the cookie or invalid, we will default
  // to the current URL path.
  // """Defaults to the path of the request URL that generated the
  //    Set-Cookie response, up to, but not including, the
  //    right-most /."""
  // How would this work for a cookie on /?  We will include it then.
  const std::string& url_path = url.path();

  size_t idx = url_path.find_last_of('/');

  // The cookie path was invalid or a single '/'.
  if (idx == 0 || idx == std::string::npos)
    return std::string("/");

  // Return up to the rightmost '/'.
  return url_path.substr(0, idx);
}

}  // namespace

CanonicalCookie::CanonicalCookie()
    : secure_(false),
      httponly_(false) {
}

CanonicalCookie::CanonicalCookie(
    const GURL& url, const std::string& name, const std::string& value,
    const std::string& domain, const std::string& path,
    const std::string& mac_key, const std::string& mac_algorithm,
    const base::Time& creation, const base::Time& expiration,
    const base::Time& last_access, bool secure, bool httponly)
    : source_(GetCookieSourceFromURL(url)),
      name_(name),
      value_(value),
      domain_(domain),
      path_(path),
      mac_key_(mac_key),
      mac_algorithm_(mac_algorithm),
      creation_date_(creation),
      expiry_date_(expiration),
      last_access_date_(last_access),
      secure_(secure),
      httponly_(httponly) {
}

CanonicalCookie::CanonicalCookie(const GURL& url, const ParsedCookie& pc)
    : source_(GetCookieSourceFromURL(url)),
      name_(pc.Name()),
      value_(pc.Value()),
      path_(CanonPath(url, pc)),
      mac_key_(pc.MACKey()),
      mac_algorithm_(pc.MACAlgorithm()),
      creation_date_(Time::Now()),
      last_access_date_(Time()),
      secure_(pc.IsSecure()),
      httponly_(pc.IsHttpOnly()) {
  if (pc.HasExpires())
    expiry_date_ = CanonExpiration(pc, creation_date_);

  // Do the best we can with the domain.
  std::string cookie_domain;
  std::string domain_string;
  if (pc.HasDomain()) {
    domain_string = pc.Domain();
  }
  bool result
      = cookie_util::GetCookieDomainWithString(url, domain_string,
                                                &cookie_domain);
  // Caller is responsible for passing in good arguments.
  DCHECK(result);
  domain_ = cookie_domain;
}

CanonicalCookie::~CanonicalCookie() {
}

std::string CanonicalCookie::GetCookieSourceFromURL(const GURL& url) {
  if (url.SchemeIsFile())
    return url.spec();

  url_canon::Replacements<char> replacements;
  replacements.ClearPort();
  if (url.SchemeIsSecure())
    replacements.SetScheme("http", url_parse::Component(0, 4));

  return url.GetOrigin().ReplaceComponents(replacements).spec();
}

// static
std::string CanonicalCookie::CanonPath(const GURL& url,
                                       const ParsedCookie& pc) {
  std::string path_string;
  if (pc.HasPath())
    path_string = pc.Path();
  return CanonPathWithString(url, path_string);
}

// static
Time CanonicalCookie::CanonExpiration(const ParsedCookie& pc,
                                      const Time& current) {
  // First, try the Max-Age attribute.
  uint64 max_age = 0;
  if (pc.HasMaxAge() &&
#ifdef COMPILER_MSVC
      sscanf_s(
#else
      sscanf(
#endif
             pc.MaxAge().c_str(), " %" PRIu64, &max_age) == 1) {
    return current + TimeDelta::FromSeconds(max_age);
  }

  // Try the Expires attribute.
  if (pc.HasExpires())
    return cookie_util::ParseCookieTime(pc.Expires());

  // Invalid or no expiration, persistent cookie.
  return Time();
}

CanonicalCookie* CanonicalCookie::Create(const GURL& url,
                                         const ParsedCookie& pc) {
  if (!pc.IsValid()) {
    return NULL;
  }

  std::string domain_string;
  if (!GetCookieDomain(url, pc, &domain_string)) {
    return NULL;
  }
  std::string path_string = CanonPath(url, pc);
  std::string mac_key = pc.HasMACKey() ? pc.MACKey() : std::string();
  std::string mac_algorithm = pc.HasMACAlgorithm() ?
      pc.MACAlgorithm() : std::string();
  Time creation_time = Time::Now();
  Time expiration_time;
  if (pc.HasExpires())
    expiration_time =  cookie_util::ParseCookieTime(pc.Expires());

  return (Create(url, pc.Name(), pc.Value(), domain_string, path_string,
                 mac_key, mac_algorithm, creation_time, expiration_time,
                 pc.IsSecure(), pc.IsHttpOnly()));
}

CanonicalCookie* CanonicalCookie::Create(const GURL& url,
                                         const std::string& name,
                                         const std::string& value,
                                         const std::string& domain,
                                         const std::string& path,
                                         const std::string& mac_key,
                                         const std::string& mac_algorithm,
                                         const base::Time& creation,
                                         const base::Time& expiration,
                                         bool secure,
                                         bool http_only) {
  // Expect valid attribute tokens and values, as defined by the ParsedCookie
  // logic, otherwise don't create the cookie.
  std::string parsed_name = ParsedCookie::ParseTokenString(name);
  if (parsed_name != name)
    return NULL;
  std::string parsed_value = ParsedCookie::ParseValueString(value);
  if (parsed_value != value)
    return NULL;

  std::string parsed_domain = ParsedCookie::ParseValueString(domain);
  if (parsed_domain != domain)
    return NULL;
  std::string cookie_domain;
  if (!cookie_util::GetCookieDomainWithString(url, parsed_domain,
                                               &cookie_domain)) {
    return NULL;
  }

  std::string parsed_path = ParsedCookie::ParseValueString(path);
  if (parsed_path != path)
    return NULL;

  std::string cookie_path = CanonPathWithString(url, parsed_path);
  // Expect that the path was either not specified (empty), or is valid.
  if (!parsed_path.empty() && cookie_path != parsed_path)
    return NULL;
  // Canonicalize path again to make sure it escapes characters as needed.
  url_parse::Component path_component(0, cookie_path.length());
  url_canon::RawCanonOutputT<char> canon_path;
  url_parse::Component canon_path_component;
  url_canon::CanonicalizePath(cookie_path.data(), path_component,
                              &canon_path, &canon_path_component);
  cookie_path = std::string(canon_path.data() + canon_path_component.begin,
                            canon_path_component.len);

  return new CanonicalCookie(url, parsed_name, parsed_value, cookie_domain,
                             cookie_path, mac_key, mac_algorithm, creation,
                             expiration, creation, secure, http_only);
}

bool CanonicalCookie::IsOnPath(const std::string& url_path) const {

  // A zero length would be unsafe for our trailing '/' checks, and
  // would also make no sense for our prefix match.  The code that
  // creates a CanonicalCookie should make sure the path is never zero length,
  // but we double check anyway.
  if (path_.empty())
    return false;

  // The Mozilla code broke this into three cases, based on if the cookie path
  // was longer, the same length, or shorter than the length of the url path.
  // I think the approach below is simpler.

  // Make sure the cookie path is a prefix of the url path.  If the
  // url path is shorter than the cookie path, then the cookie path
  // can't be a prefix.
  if (url_path.find(path_) != 0)
    return false;

  // Now we know that url_path is >= cookie_path, and that cookie_path
  // is a prefix of url_path.  If they are the are the same length then
  // they are identical, otherwise we need an additional check:

  // In order to avoid in correctly matching a cookie path of /blah
  // with a request path of '/blahblah/', we need to make sure that either
  // the cookie path ends in a trailing '/', or that we prefix up to a '/'
  // in the url path.  Since we know that the url path length is greater
  // than the cookie path length, it's safe to index one byte past.
  if (path_.length() != url_path.length() &&
      path_[path_.length() - 1] != '/' &&
      url_path[path_.length()] != '/')
    return false;

  return true;
}

bool CanonicalCookie::IsDomainMatch(const std::string& scheme,
                                    const std::string& host) const {
  // Can domain match in two ways; as a domain cookie (where the cookie
  // domain begins with ".") or as a host cookie (where it doesn't).

  // Some consumers of the CookieMonster expect to set cookies on
  // URLs like http://.strange.url.  To retrieve cookies in this instance,
  // we allow matching as a host cookie even when the domain_ starts with
  // a period.
  if (host == domain_)
    return true;

  // Domain cookie must have an initial ".".  To match, it must be
  // equal to url's host with initial period removed, or a suffix of
  // it.

  // Arguably this should only apply to "http" or "https" cookies, but
  // extension cookie tests currently use the funtionality, and if we
  // ever decide to implement that it should be done by preventing
  // such cookies from being set.
  if (domain_.empty() || domain_[0] != '.')
    return false;

  // The host with a "." prefixed.
  if (domain_.compare(1, std::string::npos, host) == 0)
    return true;

  // A pure suffix of the host (ok since we know the domain already
  // starts with a ".")
  return (host.length() > domain_.length() &&
          host.compare(host.length() - domain_.length(),
                       domain_.length(), domain_) == 0);
}

std::string CanonicalCookie::DebugString() const {
  return base::StringPrintf(
      "name: %s value: %s domain: %s path: %s creation: %"
      PRId64,
      name_.c_str(), value_.c_str(),
      domain_.c_str(), path_.c_str(),
      static_cast<int64>(creation_date_.ToTimeT()));
}

}  // namespace net
