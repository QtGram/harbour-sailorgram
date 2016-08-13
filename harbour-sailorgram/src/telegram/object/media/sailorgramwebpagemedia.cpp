#include "sailorgramwebpagemedia.h"

SailorgramWebPageMedia::SailorgramWebPageMedia(WebPageObject* webpage, QObject *parent) : QObject(parent), _webpage(webpage)
{

}


QString SailorgramWebPageMedia::url() const
{
    return this->_webpage->url();
}

QString SailorgramWebPageMedia::title() const
{
    return this->_webpage->title();
}

QString SailorgramWebPageMedia::description() const
{
    return this->_webpage->description();
}

bool SailorgramWebPageMedia::hasThumbnail() const
{
    return this->_webpage->photo()->classType() != PhotoObject::TypePhotoEmpty;
}

bool SailorgramWebPageMedia::hasPureImage() const
{
    return this->_webpage->title().isEmpty() && this->_webpage->description().isEmpty();
}
