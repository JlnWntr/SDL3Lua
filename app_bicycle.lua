WINDOW_WIDTH = 600
WINDOW_HEIGHT = 400


function APP_INIT()
    Bicycle = Lua_Load_BMP("media/bicycle.bmp")
    Background = Lua_Load_BMP("media/background.bmp")


    blit_i = 0
    blit_j = 0
    blit_duration = 10
    blit_w = 372
    blit_h = 456
    scale = 1
    blot_w = blit_w * scale
    blot_h = blit_h * scale
    blot_x = WINDOW_WIDTH/2 - blot_w/2
    blot_y = WINDOW_HEIGHT -blot_h - 120

    blit_frames  = 4 -- number of frames

    speed = 8

    background1 = {x = 0, y = 0, w = 897, h = 1024}
    background2 = {x = WINDOW_WIDTH, y = 0, w = 897, h = 1024}

end


function APP_ITERATION()
    background1.x = background1.x - speed

    background2.x = background2.x - speed

    if background1.x <= 0 - WINDOW_WIDTH then
            background1.x = background2.x + WINDOW_WIDTH
    end
    if background2.x <= 0 - WINDOW_WIDTH then
            background2.x = background1.x + WINDOW_WIDTH
    end
    Lua_Render_Texture(Background, 0, 0, background2.w, background2.h, background2.x, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
    
    Lua_Render_Texture(Background, 0, 0, background1.w, background1.h, background1.x, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
    

    Lua_Render_Texture(Bicycle, blit_i*blit_w, 0, blit_w, blit_h, blot_x, blot_y, blot_w, blot_h)
    if speed > 1 then
        blit_j = blit_j + 1
        if blit_j > blit_duration then
            blit_i = blit_i + 1
            if blit_i >= blit_frames then blit_i = 0 end
            blit_j = 0
        end
    end
    delay(10)--ms

end

function SET_SPEED(s)
    speed = 32 * (s/1023)
    --print("Speed is now " .. speed)
end

function APP_MOUSE_DOWN(x, y)
    print("app.lua: Mouse down at "  .. x .. "|" .. y)
end
