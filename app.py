import serial
import json
import csv
import time
import threading
import os
from datetime import datetime
from flask import Flask, render_template, jsonify, send_file

# --- ตั้งค่า (เช็ค Port ให้ตรงกับเครื่องคุณ!) ---
SERIAL_PORT = 'COM4'
BAUD_RATE = 115200
TEMP_CSV_FILE = 'temp_data.csv'

# ตัวแปร Global
current_data = {"ax": 0, "ay": 0, "az": 0, "gx": 0, "gy": 0, "gz": 0}
is_recording = False

app = Flask(__name__)


# --- ฟังก์ชันอ่าน Serial (ทำงานเบื้องหลัง) ---
def read_serial():
    global current_data, is_recording
    ser = None

    # พยายามเชื่อมต่อ Serial จนกว่าจะได้
    while ser is None:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            ser.setDTR(False)
            ser.setRTS(False)
            print(f"Connected to {SERIAL_PORT}")
            time.sleep(2)
        except Exception as e:
            print(f"Waiting for device... ({e})")
            time.sleep(2)

    while True:
        try:
            if ser.in_waiting > 0:
                raw_line = ser.readline().decode('utf-8', errors='ignore').strip()

                if raw_line.startswith('{') and raw_line.endswith('}'):
                    try:
                        data = json.loads(raw_line)
                        current_data = data

                        # --- บันทึกลงไฟล์ (เฉพาะตอนกด Start) ---
                        if is_recording:
                            # เวลาปัจจุบัน (ตัดให้เหลือทศนิยม 5 ตำแหน่ง)
                            now = datetime.now()
                            timestamp = now.strftime("%Y-%m-%d %H:%M:%S.%f")[:-1]

                            with open(TEMP_CSV_FILE, 'a', newline='') as f:
                                writer = csv.writer(f)
                                writer.writerow([
                                    timestamp,
                                    "{:.5f}".format(data.get('ax', 0)),
                                    "{:.5f}".format(data.get('ay', 0)),
                                    "{:.5f}".format(data.get('az', 0)),
                                    "{:.5f}".format(data.get('gx', 0)),
                                    "{:.5f}".format(data.get('gy', 0)),
                                    "{:.5f}".format(data.get('gz', 0))
                                ])
                            # print(f"Saved: {timestamp}") # เปิดบรรทัดนี้ถ้าอยากเช็คใน Console

                    except json.JSONDecodeError:
                        pass
        except Exception as e:
            print(f"Serial Error: {e}")
            time.sleep(1)


# --- Web Server Routes ---

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/data')
def get_data():
    return jsonify({
        "sensors": current_data,
        "recording": is_recording
    })


@app.route('/start')
def start_record():
    global is_recording
    # เริ่มไฟล์ใหม่ เขียนหัวตาราง
    with open(TEMP_CSV_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Timestamp', 'AX', 'AY', 'AZ', 'GX', 'GY', 'GZ'])

    is_recording = True
    print("--- Recording Started ---")
    return "Started"


@app.route('/stop')
def stop_record():
    global is_recording
    is_recording = False
    print("--- Recording Stopped ---")

    # สร้างชื่อไฟล์ตามเวลาปัจจุบัน
    filename = f"SensorData_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
    return send_file(TEMP_CSV_FILE, as_attachment=True, download_name=filename)


if __name__ == '__main__':
    # สร้าง Thread แยกสำหรับอ่าน Serial
    thread = threading.Thread(target=read_serial)
    thread.daemon = True
    thread.start()

    # รัน Web Server
    app.run(host='0.0.0.0', port=5000, debug=False)