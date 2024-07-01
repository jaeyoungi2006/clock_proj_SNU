import time
import RPi.GPIO as GPIO

class Step_Motor:
    def __init__(self, pins, rot_dir, mode, speed, speed_mode, stride_angle = 5.625 / 64):
        #Step Motor Information
        self.stride_angle = stride_angle
        self.stride_num = 360 / self.stride_angle / 8 # # of Pulse Cycle need for a single Cycle
        self.max_speed = 15 # Max Speed In RPM

        #Other Information
        #Multiple Step Modes
        self.single_step = [[int(i == j) for j in range(3, -1, -1)] for i in range(4)]
        self.full_step = [[int((i == j) or ((j - 1 - i) % 4 == 0)) for j in range(3, -1, -1)] for i in range(4)]
        self.half_step = [(self.single_step[i] if j == 0 else self.full_step[i]) for i in range(4) for j in range(2)]
        #print(*[self.single_step, self.full_step, self.half_step], sep = "\n")
        step_dic = {"Single": self.single_step, "Full": self.full_step, "Half": self.half_step}
        self.step = step_dic[mode]
        self.rot_dir = rot_dir
        
        #Pulse Time Calculation
        self.pulse_time = self.calculate_pulse_time(speed, speed_mode)
        
        GPIO.setmode(GPIO.BOARD)
        self.step_pins = pins
        for pin in self.step_pins:
            GPIO.setup(pin, GPIO.OUT)
            GPIO.output(pin, False)

        #start time & counter & action list
        self.start_time = time.time()
        self.target_time = self.start_time
        self.step_counter = 0
        self.action_list = []
        self.action_target = 0
        self.moving = False

    def calculate_pulse_time(self, speed, mode):
        if mode == "s": # Seconds Given For Single Cycle
            pass
        elif mode == "hz": #Hz Given For Single Cycle
            speed = 1 / speed
        else:
            raise Exception("InValid Speed Mode")
        if speed < 60 / self.max_speed: # Faster Than MAX SPEED.!
            speed = 60 / self.max_speed
        
        pulse_time = speed / self.stride_num / len(self.step)
        return pulse_time

    def get_next_action(self):
        if self.action_target < len(self.action_list):
            return self.action_list[self.action_target]
        else:
            self.action_target = len(self.action_list)
            return []
    
    def do(self): # Needs to be Executed every cycle
        time_from_start = time.time() - self.start_time
        time_from_target = time.time() - self.target_time
        
        next_action = self.get_next_action()
        if next_action: #valid
            if next_action[0] < time_from_start:
                if next_action[1] in ["Move", "Stop"]:
                    self.moving = (next_action[1] == "Move")
                elif next_action[1] in ["CW", "CCW"]:
                    self.rot_dir = next_action[1]
                elif next_action[1] in ["Speed"]:
                    if len(next_action) == 3:
                        self.pulse_time = self.calculate_pulse_time(next_action[2], next_action[3])
                self.action_target += 1
                
        if self.moving:
            if time.time() > self.target_time:
                for i in range(4):
                    selectedpin = self.step_pins[i]
                    GPIO.output(selectedpin, bool(self.step[self.step_counter][i]))
                self.step_counter += (1 if self.rot_dir == "CW" else -1)
                self.step_counter = self.step_counter % len(self.step)
                self.target_time += self.pulse_time
        
        

if __name__ == "__main__":
    step_motor1 = Step_Motor([8, 10, 12, 16], "CW", "Half", 10, "s")
    step_motor1.action_list = [[0, "Move"], [10, "Stop"], [15, "CCW"], [15, "Speed", 20, "s"], [15, "Move"], [20, "Stop"]]
    # 0~10s CW circle, 15~20s CCW quarter circle
    try:
        while True:
            step_motor1.do()
    except KeyboardInterrupt:
        GPIO.cleanup()
        
