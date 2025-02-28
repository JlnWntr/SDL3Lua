WINDOW_WIDTH = 400
WINDOW_HEIGHT = 600

function APP_INIT()
    Lua_Load_Spritelibrary("sprite.bmp") --from https://opengameart.org/content/a-platformer-in-the-forest
end

local i = 0
local const blit_w = 32
local const blit_h = 28

local const blot_w = blit_w * 16
local const blot_h = blit_h * 16
local const blot_x = WINDOW_WIDTH/2 - blot_w/2
local const blot_y = WINDOW_HEIGHT/2 -blot_h/2

local const blits  = 4 -- number of frames

function APP_ITERATION()
    Lua_Render_Sprite(i*blit_w, 0, blit_w, blit_h, blot_x, blot_y, blot_w, blot_h)
    i = i + 1
    if i >= blits then i = 0 end
    delay(100)--ms
end


function APP_MOUSE_DOWN(x, y)
    print("app.lua: Mouse down at "  .. x .. "|" .. y)
end
