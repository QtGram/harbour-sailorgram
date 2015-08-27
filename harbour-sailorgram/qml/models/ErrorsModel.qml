import QtQuick 2.1

ListModel
{
    id: errorsmodel

    function addError(errorcode, functionname, errortext) {
        errorsmodel.insert(0, { "errorcode": errorcode,
                                "functionname": functionname,
                                "errortext": errortext });
    }
}
