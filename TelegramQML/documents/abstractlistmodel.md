# AbstractListModel

 * [Component details](#component-details)
 * [Normal Properties](#normal-properties)
 * [Methods](#methods)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>AbstractListModel</font>|
|C++ class|<font color='#074885'>TelegramAbstractListModel</font>|
|Inherits|<font color='#074885'>QAbstractListModel</font>|
|Model|<font color='#074885'>Yes</font>|



### Normal Properties

* <font color='#074885'><b>count</b></font>: int (readOnly)
* <font color='#074885'><b>isEmpty</b></font>: boolean (readOnly)
* <font color='#074885'><b>errorText</b></font>: string (readOnly)
* <font color='#074885'><b>errorCode</b></font>: int (readOnly)
* <font color='#074885'><b>items</b></font>: [QQmlListProperty<Q>](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/qqmllistproperty<q>.md) (readOnly)


### Methods

 * variant <font color='#074885'><b>get</b></font>(int index, int role)
 * variant <font color='#074885'><b>get</b></font>(int index, string roleName)
 * map <font color='#074885'><b>get</b></font>(int index)
 * int <font color='#074885'><b>indexOf</b></font>(int role, variant value)



