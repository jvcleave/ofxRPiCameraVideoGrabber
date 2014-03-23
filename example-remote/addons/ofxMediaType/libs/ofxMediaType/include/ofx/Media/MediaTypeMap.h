// =============================================================================
//
// Copyright (c) 2009-2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include <sstream>
#include <string>
#include <map>
#include "Poco/File.h"
#include "Poco/Mutex.h"
#include "Poco/Path.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/MediaType.h"
#include "ofx/Media/BaseMediaTypeProvider.h"


namespace ofx {
namespace Media {


class MediaTypeMap: public BaseMediaTypeProvider
{
public:
    typedef std::shared_ptr<MediaTypeMap> SharedPtr;
    typedef std::map<std::string,Poco::Net::MediaType> FileSuffixToMediaTypeMap;
    typedef FileSuffixToMediaTypeMap::const_iterator ConstIterator;

    MediaTypeMap(const std::string& mimeTypesFile = "mime.types");
    virtual ~MediaTypeMap();

    Poco::Net::MediaType getMediaTypeForPath(const Poco::Path& path) const;
    std::string getMediaDescription(const Poco::Path& path, bool bExamineCompressed) const;

    void add(const std::string& suffix, const Poco::Net::MediaType& mediaType);

    void load(std::istream& inputStream);

    void clear();

    Poco::Net::MediaType getDefaultMediaType() const;
    void setDefaultMediaType(const Poco::Net::MediaType& defaultMediaType);

    static SharedPtr getDefault()
    {
        static SharedPtr ptr = SharedPtr(new MediaTypeMap());
        return ptr;
    }

    static FileSuffixToMediaTypeMap parse(std::istream& inputStream);

    static const std::string DEFAULT_MEDIA_TYPE;

private:
    FileSuffixToMediaTypeMap _map;
    Poco::Net::MediaType _defaultMediaType;

};


} } // namespace ofx::Media
