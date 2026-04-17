import os

svg_led_on = '''<svg width="100" height="100" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg"><circle cx="50" cy="50" r="40" fill="#00FF00" stroke="#00AA00" stroke-width="5"/></svg>'''
svg_led_off = '''<svg width="100" height="100" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg"><circle cx="50" cy="50" r="40" fill="#333333" stroke="#111111" stroke-width="5"/></svg>'''
svg_buzzer_on = '''<svg width="100" height="100" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg"><path d="M20,30 L40,30 L60,10 L60,90 L40,70 L20,70 Z" fill="#FF5555"/><path d="M70,30 Q90,50 70,70" fill="none" stroke="#FF5555" stroke-width="5"/></svg>'''
svg_buzzer_off = '''<svg width="100" height="100" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg"><path d="M20,30 L40,30 L60,10 L60,90 L40,70 L20,70 Z" fill="#555555"/></svg>'''
svg_sensor = '''<svg width="100" height="100" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg"><rect x="20" y="20" width="60" height="60" fill="#4488FF" rx="10"/><circle cx="50" cy="50" r="15" fill="#FFFFFF"/></svg>'''

base_dir = "res/images/hwctrl/"
with open(base_dir + "led_on.svg", "w") as f: f.write(svg_led_on)
with open(base_dir + "led_off.svg", "w") as f: f.write(svg_led_off)
with open(base_dir + "buzzer_on.svg", "w") as f: f.write(svg_buzzer_on)
with open(base_dir + "buzzer_off.svg", "w") as f: f.write(svg_buzzer_off)
with open(base_dir + "sensor.svg", "w") as f: f.write(svg_sensor)
print("SVGs created.")
