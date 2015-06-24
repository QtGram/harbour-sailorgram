.pragma library

function userName(user)
{
    return ((user.username.length > 0) ? user.username : (user.firstName + " " + user.lastName));
}
