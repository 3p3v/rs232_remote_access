function sign(callback, username, password)
{
    // create uri
    var uri = location.host + "/auth/jwt/sign";

    // request def
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
        {
            callback(xmlHttp.responseText);
        }
        else if(xmlHttp.readyState == 4 && xmlHttp.status == 401) 
        {

        }
        else
        {
            return -1;
        }

        return xmlHttp.status;
    } 
        
    xmlHttp.open("GET", uri, true); // true for asynchronous 
    xmlHttp.send(null);
}
