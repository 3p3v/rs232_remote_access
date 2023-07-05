local jwt = require "resty.jwt"
local validators = require "resty.jwt-validators"
local cjson = require "cjson"
local mysql = require "resty.mysql"

-- specify response encoding
ngx.header.content_type = "application/json; charset=utf-8"

-- specify key
local key = "secret"

-- read token
local jwtToken = ngx.var.http_authorization

-- check if user sent token
if jwtToken == nil then
  ngx.status = ngx.HTTP_UNAUTHORIZED
  ngx.say("{\"error\": \"Forbidden\"}")
  ngx.exit(ngx.HTTP_UNAUTHORIZED)
end

-- check expiration
local claim_spec = {
  exp = validators.is_not_expired()-- To check expiry
}
local jwt_obj = jwt:verify(key, jwtToken, claim_spec)
if not jwt_obj["verified"] then
  ngx.status = ngx.HTTP_UNAUTHORIZED
  ngx.say("{\"error\": \"INVALID_JWT\"}")
  ngx.exit(ngx.HTTP_UNAUTHORIZED)
end

-- init database
local db, err = mysql:new()
if not db then
    ngx.exit(ngx.HTTP_INTERNAL_SERVER_ERROR)
    return
end
db:set_timeout(1000) -- 1 sec

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

-- get username from token
local username = jwt_obj.payload.username


-- generate jwt
local time = ngx.now() + 15 * 60
local jwt_token = jwt:sign(
    key,
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
