# StickersModel

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Methods](#methods)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>StickersModel</font>|
|C++ class|<font color='#074885'>TelegramStickersModel</font>|
|Inherits|<font color='#074885'>[AbstractEngineListModel](abstractenginelistmodel.md)</font>|
|Model|<font color='#074885'>Yes</font>|


### Required Properties

* <font color='#074885'><b>engine</b></font>: [Engine](engine.md)
* <font color='#074885'><b>documents</b></font>: list&lt;variant&gt;


### Normal Properties

* <font color='#074885'><b>refreshing</b></font>: boolean (readOnly)
* <font color='#074885'><b>shortName</b></font>: string
* <font color='#074885'><b>stickerSet</b></font>: [InputStickerSet](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputstickerset.md)


### Methods

 * void <font color='#074885'><b>showRecents</b></font>()
 * void <font color='#074885'><b>addToRecents</b></font>([Document](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/document.md) doc)



### Enumerator


##### DialogsRoles

|Key|Value|
|---|-----|
|RoleDocument|256|
|RoleAlt|257|


### Roles

 * model.<font color='#074885'>document</font>
 * model.<font color='#074885'>alt</font>
