import os
from PIL import Image, ImageDraw

def make_icon(name, type_):
    img = Image.new('RGBA', (64, 64), (0,0,0,0))
    d = ImageDraw.Draw(img)
    if type_ == "close":
        d.line([16,16, 48,48], fill="white", width=6)
        d.line([16,48, 48,16], fill="white", width=6)
    elif type_ == "minimize":
        d.line([12,32, 52,32], fill="white", width=6)
    elif type_ == "menu":
        d.line([12,16, 52,16], fill="white", width=6)
        d.line([12,32, 52,32], fill="white", width=6)
        d.line([12,48, 52,48], fill="white", width=6)
    elif type_ == "folder":
        # Draw it with lines to avoid pillow version issues
        pts = [(8,16), (24,16), (30,24), (56,24), (56,48), (8,48), (8,16)]
        for i in range(len(pts)-1):
            d.line([pts[i], pts[i+1]], fill="white", width=4)
        d.line([8,24, 30,24], fill="white", width=4)
    elif type_ == "play":
        d.polygon([20,12, 20,52, 50,32], fill="white")
    elif type_ == "pause":
        d.rectangle([16,16, 26,48], fill="white")
        d.rectangle([38,16, 48,48], fill="white")
    img.save(name)

os.makedirs("res/images/music", exist_ok=True)
make_icon("res/images/music/close_new.png", "close")
make_icon("res/images/music/minimize_new.png", "minimize")
make_icon("res/images/music/menu_new.png", "menu")
make_icon("res/images/music/folder_new.png", "folder")

os.makedirs("res/images/video", exist_ok=True)
make_icon("res/images/video/close_new.png", "close")
make_icon("res/images/video/minimize_new.png", "minimize")
make_icon("res/images/video/menu_new.png", "menu")
make_icon("res/images/video/folder_new.png", "folder")
make_icon("res/images/video/play_new.png", "play")
make_icon("res/images/video/pause_new.png", "pause")
