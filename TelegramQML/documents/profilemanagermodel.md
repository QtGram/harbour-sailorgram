# ProfileManagerModel

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
|Component|<font color='#074885'>ProfileManagerModel</font>|
|C++ class|<font color='#074885'>TelegramProfileManagerModel</font>|
|Inherits|<font color='#074885'>[AbstractListModel](abstractlistmodel.md)</font>|
|Model|<font color='#074885'>Yes</font>|


### Required Properties

* <font color='#074885'><b>source</b></font>: string
* <font color='#074885'><b>engineDelegate</b></font>: Component


### Normal Properties

* <font color='#074885'><b>initializing</b></font>: boolean (readOnly)


### Methods

 * void <font color='#074885'><b>addNew</b></font>()
 * int <font color='#074885'><b>add</b></font>(string phoneNumber, boolean mute, [Engine](engine.md) engine)
 * boolean <font color='#074885'><b>remove</b></font>(string phoneNumber)



### Enumerator


##### DataRole

|Key|Value|
|---|-----|
|DataPhoneNumber|256|
|DataMute|257|
|DataEngine|258|

##### AddResult

|Key|Value|
|---|-----|
|AddResultSucessfully|0|
|AddResultExists|1|
|AddResultInvalidPhone|2|
|AddResultDatabaseError|3|


### Roles

 * model.<font color='#074885'>phoneNumber</font>
 * model.<font color='#074885'>mute</font>
 * model.<font color='#074885'>engine</font>
