from PIL import Image, ImageDraw

def create_icon(name, is_close=False, is_menu=False):
    img = Image.new('RGBA', (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    
    if is_close:
        # Red circle, white cross
        d.ellipse([0,0, 64,64], fill="#FF5F56")
        d.line([20,20, 44,44], fill="white", width=4)
        d.line([20,44, 44,20], fill="white", width=4)
    elif is_menu:
        # Transparent background, white lines
        d.line([12,18, 52,18], fill="white", width=4)
        d.line([12,32, 52,32], fill="white", width=4)
        d.line([12,46, 52,46], fill="white", width=4)
    else:
        # Yellow circle, white minus
        d.ellipse([0,0, 64,64], fill="#FFBD2E")
        d.line([20,32, 44,32], fill="white", width=6)
        
    img.save(name)

create_icon("res/images/music/minimize_new.png")
create_icon("res/images/music/close_new.png", is_close=True)
create_icon("res/images/music/menu_new.png", is_menu=True)

# Also create for video if needed
import os
os.makedirs("res/images/video", exist_ok=True)
create_icon("res/images/video/minimize_new.png")
create_icon("res/images/video/close_new.png", is_close=True)
create_icon("res/images/video/menu_new.png", is_menu=True)
