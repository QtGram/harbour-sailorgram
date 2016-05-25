# MembersListModel

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>MembersListModel</font>|
|C++ class|<font color='#074885'>TelegramMembersListModel</font>|
|Inherits|<font color='#074885'>[AbstractEngineListModel](abstractenginelistmodel.md)</font>|
|Model|<font color='#074885'>Yes</font>|


### Required Properties

* <font color='#074885'><b>engine</b></font>: [Engine](engine.md)
* <font color='#074885'><b>currentPeer</b></font>: [InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md)


### Normal Properties

* <font color='#074885'><b>filter</b></font>: int
* <font color='#074885'><b>refreshing</b></font>: boolean (readOnly)
* <font color='#074885'><b>dateConvertorMethod</b></font>: function(){[code]}




### Enumerator


##### DataRoles

|Key|Value|
|---|-----|
|RoleDate|256|
|RoleInviter|257|
|RoleUser|258|
|RoleKickedBy|259|
|RoleType|260|
|RolePeer|261|
|RoleStatus|262|

##### Types

|Key|Value|
|---|-----|
|TypeNormal|0|
|TypeSelf|1|
|TypeAdmin|2|
|TypeModerator|3|
|TypeEditor|4|
|TypeKicked|5|
|TypeCreator|6|
|TypeUnknown|7|


### Roles

 * model.<font color='#074885'>date</font>
 * model.<font color='#074885'>inviter</font>
 * model.<font color='#074885'>user</font>
 * model.<font color='#074885'>kickedBy</font>
 * model.<font color='#074885'>type</font>
 * model.<font color='#074885'>peer</font>
 * model.<font color='#074885'>status</font>
