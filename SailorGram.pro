TEMPLATE = subdirs

webp-plugin.file = webp-plugin/webp.pro
libqtelegram.file = LibQTelegram/LibQTelegram.pro

harbour-sailorgram.depends = webp-plugin libqtelegram

SUBDIRS += webp-plugin libqtelegram harbour-sailorgram
