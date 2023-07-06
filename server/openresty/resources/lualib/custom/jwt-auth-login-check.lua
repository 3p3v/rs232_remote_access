local jwt = require "resty.jwt"
local validators = require "resty.jwt-validators"
local cjson = require "cjson"

ngx.header.content_type = "application/json; charset=utf-8"

local key = "secret"

-- if ngx.var.request_method ~= "OPTIONS" and not string.match(ngx.var.uri, "login") then
local jwtToken = ngx.var.cookie_Authorization
if jwtToken == nil then
    ngx.say("{\"error\": \"Forbidden\"}")
    ngx.exit(ngx.HTTP_UNAUTHORIZED)
end
local claim_spec = {
  exp = validators.is_not_expired()-- To check expiry
}
local jwt_obj = jwt:verify(key, jwtToken, claim_spec)
if not jwt_obj["verified"] then
    ngx.say("{\"error\": \"INVALID_JWT\"}")
    ngx.exit(ngx.HTTP_UNAUTHORIZED)
end

ngx.say(cjson.encode({"message" = "Authorized"}))
ngx.exit(ngx.HTTP_OK)