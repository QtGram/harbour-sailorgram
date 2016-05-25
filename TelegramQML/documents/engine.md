# Engine

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Methods](#methods)
 * [Signals](#signals)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>Engine</font>|
|C++ class|<font color='#074885'>TelegramEngine</font>|
|Inherits|<font color='#074885'>[Tq](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/tq.md)</font>|
|Model|<font color='#074885'>No</font>|


### Required Properties

* <font color='#074885'><b>phoneNumber</b></font>: string
* <font color='#074885'><b>configDirectory</b></font>: string
* <font color='#074885'><b>app</b></font>: [App](app.md)
* <font color='#074885'><b>host</b></font>: [Host](host.md)


### Normal Properties

* <font color='#074885'><b>cache</b></font>: [Cache](cache.md)
* <font color='#074885'><b>authStore</b></font>: [AuthStore](authstore.md)
* <font color='#074885'><b>profileManager</b></font>: [ProfileManagerModel](profilemanagermodel.md)
* <font color='#074885'><b>our</b></font>: [UserFull](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/userfull.md) (readOnly)
* <font color='#074885'><b>timeout</b></font>: int
* <font color='#074885'><b>state</b></font>: int (readOnly)
* <font color='#074885'><b>logLevel</b></font>: int
* <font color='#074885'><b>tempPath</b></font>: string


### Methods

 * void <font color='#074885'><b>logout</b></font>()


### Signals

 * void <font color='#074885'><b>authLoggedIn</b></font>()
 * void <font color='#074885'><b>authNeeded</b></font>()
 * void <font color='#074885'><b>authLoggedOut</b></font>()
 * void <font color='#074885'><b>telegramChanged</b></font>()


### Enumerator


##### AuthState

|Key|Value|
|---|-----|
|AuthUnknown|0|
|AuthInitializing|1|
|AuthNeeded|2|
|AuthFetchingOurDetails|3|
|AuthLoggedIn|4|

##### LogLevel

|Key|Value|
|---|-----|
|LogLevelClean|0|
|LogLevelUseful|1|
|LogLevelFull|2|

