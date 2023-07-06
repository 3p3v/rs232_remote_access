async function auth_sign(username, password)
{
    // create uri
    var uri = "http://" + location.host + "/auth/sign";

    // request def
    const resp = await fetch(uri, {
      method: 'post', // Default is 'get'
      body: JSON.stringify({ "username": username, "password": password}),
      mode: 'cors',
      headers: new Headers({
        'Content-Type': 'application/json'
      })
    })

    console.log(resp.status);

    return resp.status;
}

async function auth_check(username, password)
{
    // create uri
    var uri = "http://" + location.host + "/auth/check";

    // request def
    const resp = await fetch(uri, {
      method: 'post', // Default is 'get'
      // body: JSON.stringify({ "username": username, "password": password}),
      mode: 'cors',
      headers: new Headers({
        'Content-Type': 'application/json'
      })
    })

    console.log(resp.status);

    return resp.status;
}
