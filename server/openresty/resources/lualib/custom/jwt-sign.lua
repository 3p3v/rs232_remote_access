local jwt = require "resty.jwt"
-- local validators = require "resty.jwt-validators"
local cjson = require "cjson"
local mysql = require "resty.mysql"
local bcrypt = require( "bcrypt" )

-- specify response encoding
ngx.header.content_type = "application/json; charset=utf-8"

-- specify key
local key = "secret"

-- init database
local db, err = mysql:new()
if not db then
    ngx.exit(ngx.HTTP_INTERNAL_SERVER_ERROR)
    return
end

db:set_timeout(10000) -- 1 sec

-- connect to database
local ok, err, errcode, sqlstate = db:connect{
    host = "db",
    port = 3306,
    database = "db",
    user = "auth_user",
    password = "auth_user_pass",
    charset = "utf8",
    max_packet_size = 1024 * 1024,
}

if not ok then
    ngx.exit(ngx.HTTP_INTERNAL_SERVER_ERROR)
    return
end

-- get username and password from request body
ngx.req.read_body()
local encoded_body = ngx.req.get_body_data()
local request_body = cjson.decode(encoded_body)
local username = request_body.username
local password = request_body.password

-- get credentials from database
local res, err, errcode, sqlstate =
    db:query("select username, password from users where username = \'" .. username .. "\' limit 1")
if not res then
    ngx.exit(ngx.HTTP_INTERNAL_SERVER_ERROR)
    return
end

-- user not found
if table.getn(res) != 1 then
    ngx.exit(ngx.HTTP_UNAUTHORIZED)
    return
end

-- validate password
local bcrypted_password = res[1].password
-- local log_rounds = 10
-- local valid = bcrypt.digest( password, log_rounds )
local valid = bcrypt.verify( password, bcrypted_password )
if not valid then
    ngx.exit(ngx.HTTP_UNAUTHORIZED)
    return
end

-- generate jwt
local time = ngx.now() + 15 * 60
local jwt_token = jwt:sign(
    "lua-resty-jwt",
    {
        header = {typ = "JWT", alg = "HS512"},
        payload = {username = username, exp = time} -- os.time()
    }
)

-- close connection with database
local ok, err = db:close()
if not ok then
    ngx.say("close")
    ngx.exit(ngx.HTTP_INTERNAL_SERVER_ERROR)
    return
end

-- return token
local response_body = cjson.encode({
    authorization = jwt_token
})
ngx.say(response_body)
ngx.exit(ngx.HTTP_OK)
