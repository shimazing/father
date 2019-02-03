from flask import Flask
from flask import request, jsonify
import os
import json
import datetime
MAX_LEN = 120
app = Flask(__name__)
app.config['JSON_AS_ASCII'] = False
ROOT_DIR = os.path.dirname(os.getcwd())
DATA_DIR = os.path.join(ROOT_DIR, 'data')
N_STATIONS = 5
@app.route("/")
def show_all():
    state_all = []
    #todo = next(os.walk(os.path.join(ROOT_DIR, DATA_DIR)))[2]
    for station_id in range(N_STATIONS):
        path = os.path.join(DATA_DIR, 'station_{:02d}'.format(station_id))
        with open(path, 'r') as file:
            data = json.load(file)
        #name = data['name']
        #timestamp = data['time_stamp']
        log = eval(data['log'])
        temperature = log[0]['temperature']
        humidity = log[0]['humidity']
        timestamp = log[0]['timestamp']
        name = log[0]['name']
        state_all.append({'station_id': station_id,
                          'station_name': name,
                          'temperature': temperature,
                          'humidity': humidity,
                          'timestamp': timestamp.strftime('%Y-%m-%d %H:%M:%S')})
    return jsonify(state_all=state_all)


@app.route('/station/<station_id>')
def show_station_status(station_id):
    station_id = int(station_id)
    path = os.path.join(DATA_DIR, 'station_{:02d}'.format(station_id))
    with open(path, 'r') as file:
        accum_data = json.load(file)
    #name = data['name']
    #timestamp = data['time_stamp']
    log = eval(accum_data['log'])

    temperature = log[0]['temperature']
    humidity = log[0]['humidity']
    timestamp = log[0]['timestamp']
    name = log[0]['name']
    print(log[0])
    return jsonify(log=log)
    #return "{} | {} | Temperature: {}*C | Humidity: {}%".format(name, timestamp.strftime('%Y-%m-%d %H:%M:%S'), temperature, humidity)
    #return jsonify(data), 200


@app.route('/save_state', methods=['POST'])
def save_state():
    if request.method == 'POST':
        data = request.get_json(silent=True)
        data["timestamp"] = datetime.datetime.now()
        #print(data)
        id = int(data['id'])
        path = os.path.join(DATA_DIR, 'station_{:02d}'.format(id))
        if os.path.exists(path):
            with open(path, 'r') as file:
                accum_data = json.load(file)
        else:
            accum_data = {'id': str(id), 'log': "[]"}
        log = eval(accum_data['log'])
        log = [data] + log
        log[:] = log[:MAX_LEN]
        accum_data['log'] = str(log)
        try:
            with open(path, 'w') as file:
                json.dump(accum_data, file)
            return "Success", 200
        except Exception as e:
            return "Fail " + str(e) + str(data), 404



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=2203, debug=True)
