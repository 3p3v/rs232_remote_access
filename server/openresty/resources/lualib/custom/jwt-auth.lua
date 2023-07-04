local jwt = require "resty.jwt"
local validators = require "resty.jwt-validators"
local cjson = require "cjson"

local key = "secret"

if ngx.var.request_method ~= "OPTIONS" and not string.match(ngx.var.uri, "login") then
  local jwtToken = ngx.var.http_authorization
  if jwtToken == nil then
    ngx.status = ngx.HTTP_UNAUTHORIZED
    ngx.header.content_type = "application/json; charset=utf-8"
    ngx.say("{\"error\": \"Forbidden\"}")
    ngx.exit(ngx.HTTP_UNAUTHORIZED)
  end
  local claim_spec = {
    exp = validators.is_not_expired()-- To check expiry
  }
  local jwt_obj = jwt:verify(key, jwtToken, claim_spec)
  if not jwt_obj["verified"] then
    ngx.status = ngx.HTTP_UNAUTHORIZED
    ngx.header.content_type = "application/json; charset=utf-8"
    ngx.say("{\"error\": \"INVALID_JWT\"}")
    ngx.exit(ngx.HTTP_UNAUTHORIZED)
  end

  -- Auth completed, remove authorization and add username to header
  -- ngx.say(cjson.encode(jwt_obj))
  local username = jwt_obj.payload.username
  -- ngx.say(username)
  -- ngx.exit(ngx.HTTP_OK)
  ngx.req.clear_header("authorization")
  ngx.req.set_header("username", username)
end