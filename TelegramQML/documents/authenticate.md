# Authenticate

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
|Component|<font color='#074885'>Authenticate</font>|
|C++ class|<font color='#074885'>TelegramAuthenticate</font>|
|Inherits|<font color='#074885'>[Tq](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/tq.md)</font>|
|Model|<font color='#074885'>No</font>|


### Required Properties

* <font color='#074885'><b>engine</b></font>: [Engine](engine.md)


### Normal Properties

* <font color='#074885'><b>state</b></font>: int (readOnly)
* <font color='#074885'><b>callTimeout</b></font>: int (readOnly)
* <font color='#074885'><b>remainingTime</b></font>: int (readOnly)


### Methods

 * void <font color='#074885'><b>signUp</b></font>(string firstName, string lastName)
 * void <font color='#074885'><b>signIn</b></font>(string code)
 * void <font color='#074885'><b>checkPassword</b></font>(string password)


### Signals

 * void <font color='#074885'><b>authLoggedIn</b></font>()


### Enumerator


##### AuthState

|Key|Value|
|---|-----|
|AuthUnknown|0|
|AuthCheckingPhone|1|
|AuthCheckingPhoneError|2|
|AuthSignUpNeeded|3|
|AuthCodeRequesting|4|
|AuthCodeRequestingError|5|
|AuthCodeRequested|6|
|AuthPasswordRequested|7|
|AuthLoggingIn|8|
|AuthLoggingInError|9|
|AuthLoggedIn|10|

