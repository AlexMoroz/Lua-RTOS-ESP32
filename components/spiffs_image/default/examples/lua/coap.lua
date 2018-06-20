-- ----------------------------------------------------------------
--
-- Lua RTOS examples
-- ----------------------------------------------------------------
-- This sample starts a lua server and handle POST, GET, PUT 
-- requests
-- ----------------------------------------------------------------
types = {"GET", "POST", "PUT"}

pt.coap.addHandler("GET", function (url) 
  print(url)
end)

pt.coap.addHandler("POST", function (url, data) 
  print(data)
end)

pt.coap.start("name")