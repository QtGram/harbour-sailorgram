# DownloadHandler

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Methods](#methods)
 * [Signals](#signals)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>DownloadHandler</font>|
|C++ class|<font color='#074885'>TelegramDownloadHandler</font>|
|Inherits|<font color='#074885'>[Tq](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/tq.md)</font>|
|Model|<font color='#074885'>No</font>|


### Required Properties

* <font color='#074885'><b>source</b></font>: [TypeQObject](typeqobject.md)


### Normal Properties

* <font color='#074885'><b>target</b></font>: [FileLocation](filelocation.md) (readOnly)
* <font color='#074885'><b>targetType</b></font>: int (readOnly)
* <font color='#074885'><b>size</b></font>: int (readOnly)
* <font color='#074885'><b>downloadedSize</b></font>: int (readOnly)
* <font color='#074885'><b>downloadTotal</b></font>: int (readOnly)
* <font color='#074885'><b>downloading</b></font>: boolean (readOnly)
* <font color='#074885'><b>destination</b></font>: string (readOnly)
* <font color='#074885'><b>thumbnail</b></font>: string (readOnly)
* <font color='#074885'><b>imageSize</b></font>: size (readOnly)
* <font color='#074885'><b>thumbnailSize</b></font>: size (readOnly)


### Methods

 * boolean <font color='#074885'><b>download</b></font>()
 * boolean <font color='#074885'><b>check</b></font>()
 * void <font color='#074885'><b>stop</b></font>()


### Signals

 * void <font color='#074885'><b>engineChanged</b></font>()


