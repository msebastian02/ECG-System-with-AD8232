import pyrebase
from flask import Flask
from flask import request
from flask_cors import CORS
from Crypto.Cipher import AES
from textwrap import wrap

key = 'PASSWORD12345678'

Config = {
    "apiKey": "AIzaSyA9dU2W5hgNG0MV-vGalY1JVWy-Fa6BVIA",
    "authDomain": "ecgdb-usta.firebaseapp.com",
    "databaseURL": "https://ecgdb-usta.firebaseio.com",
    "projectId": "ecgdb-usta",
    "storageBucket": "ecgdb-usta.appspot.com",
    "messagingSenderId": "110203585749",
    "appId": "1:110203585749:web:b16053164283cf6b048e5c",
    "measurementId": "G-WSL94FJNZ7"
}

firebase = pyrebase.initialize_app(Config)
db = firebase.database()

app = Flask(__name__)
CORS(app)

@app.route('/', methods=['GET'])
def basic():
    identification = request.args.get("id")
    keys = list(db.child("Data").child(identification).shallow().get().val())
    keys.remove('DateTime')
    samples = ''
    for var in keys:
        samples = samples + str(db.child("Data").child(identification).child(var).get().val())
    data = bytes.fromhex(samples)
    # Desencripta el dato recibido
    decipher = AES.new(key.encode('utf-8'), AES.MODE_ECB)
    msg_dec = decipher.decrypt(data).hex()
    msg_dec = wrap(msg_dec, 4)
    for x in range(len(msg_dec)):
        msg_dec[x] = int(msg_dec[x], 16) * 3 / 4096
    while msg_dec[len(msg_dec) - 1] == 0:
        msg_dec.pop()
    msg_dec = str(msg_dec)
    msg_dec = msg_dec.translate({ord('['): None})
    msg_dec = msg_dec.translate({ord(']'): None})
    ECGJson = dict({"Samples": msg_dec})
    # Retorna dato desencriptado
    print("Datos enviados correctamente")
    return ECGJson

@app.route('/login', methods=['GET'])
def requestLog():
    try:
        # Recibe parametro y lee de base de datos
        user = request.args.get("user").split(',')
        print('Data in: ', user)
        password_db = db.child("AdminUsers").child(user[0]).child("Password").get().val()
        isAccepted = password_db == int(user[1])
        if(isAccepted):
            db.child("AdminUsers").child(user[0]).update({"Log": "True"})     
        return dict({'data': isAccepted})
    except:
        print('An exception occurred')

@app.route('/islogin', methods=['GET'])
def requestIsLog():
    isLogin = False
    name = ''
    all_users = db.child("AdminUsers").get()
    for user in all_users.each():
        if(user.val()['Log'] == 'True'):
            isLogin = True
            name = user.key()
    print('Data out: ', isLogin)
    return dict({'data': isLogin, 'name': name})

@app.route('/logout', methods=['GET'])
def requestLogOut():
    try:
        # Recibe parametro y lee de base de datos
        username = request.args.get("username")
        db.child("AdminUsers").child(username).update({"Log": "False"}) 
        return dict({'data': True})
    except:
        print("An exception occurred")

@app.route('/users', methods=['GET'])
def requestUsers():
    users = []
    all_users = db.child("Users").get()
    for user in all_users.each():
        users.append(dict({
            'name': user.key()
        }))
    return dict({'data': users})

@app.route('/userinfo', methods=['GET'])
def requestUserInfo():
    # Recibe parametro y lee de base de datos
    username = request.args.get("username")
    user_keys = list(db.child("Users").child(username).shallow().get().val())
    user_info = db.child("Users").child(username).get()
    user_keys.remove('Address')
    user_keys.remove('Age')
    user_keys.remove('Email')
    user_keys.remove('Phone')
    user = dict({
        'name': user_info.key(),
        'address': user_info.val()['Address'],
        'age': user_info.val()['Age'],
        'email': user_info.val()['Email'],
        'phone': user_info.val()['Phone'],
        'samples': []
    })
    for sample_key in user_keys:
        sample = dict({
            'date': db.child("Users").child(username).child(sample_key).child('DateTime').get().val(),
            'id': sample_key
        })
        user['samples'].append(sample)

    return dict({'data': user})

@app.route('/samples', methods=['GET'])
def requestSamples():
    samples = []
    samples_keys = list(db.child("Data").shallow().get().val())
    for sample_key in samples_keys:
        samples.append(dict({
            'id': sample_key,
            'date': db.child("Data").child(sample_key).child('DateTime').get().val()
        }))
    return dict({'data': samples})

@app.route('/deletesample', methods=['GET'])
def requestDeleteSample():
    sample_key = request.args.get("sampleid").split(',')
    user_key = sample_key[1]
    sample_key = sample_key[0]
    print('Info: ', user_key, sample_key)
    db.child("Users").child(user_key).child(sample_key).remove()
    return dict({'data': True})

@app.route('/newsamples', methods=['GET'])
def requestNewSamples():    
    samples = request.args.get("samples").split(',')
    user = samples[len(samples)-1]
    samples.remove(user)
    print('Data in: ', samples)
    for sample_key in samples:
        data_keys = list(db.child("Data").child(sample_key).get().val())
        sample_dict = dict({
        })
        for data_key in data_keys:
            sample_dict[data_key] = db.child("Data").child(sample_key).child(data_key).get().val()
        db.child("Users").child(user).push(sample_dict)
        db.child("Data").child(sample_key).remove()
    return dict({'data': True})

@app.route('/newuser', methods=['GET'])
def requestNewUser():
    user_info = request.args.get("userinfo").split(',')
    info_db = db.child("Users").get().val()
    data = dict({
        'Age': user_info[1],
        'Email': user_info[2],
        'Phone': user_info[3],
        'Address': user_info[4]
    })
    info_db[user_info[0]] = data
    db.child("Users").set(info_db)
    return dict({'data': True})

@app.route('/data2graph', methods=['GET'])
def requestData2Graph():
    info = request.args.get("info").split(',')
    data_keys = list(db.child("Users").child(info[0]).child(info[1]).shallow().get().val())
    data_keys.remove('DateTime')
    data = ''
    for data_key in data_keys:
        data = data + str(db.child("Users").child(info[0]).child(info[1]).child(data_key).get().val())
    data = bytes.fromhex(data)
    # Desencripta el dato recibido
    decipher = AES.new(key.encode('utf-8'), AES.MODE_ECB)
    msg_dec = decipher.decrypt(data).hex()
    msg_dec = wrap(msg_dec, 4)
    print(msg_dec)
    for x in range(len(msg_dec)):
        msg_dec[x] = int(msg_dec[x], 16) * 3 / 4096
    pos=0
    while pos<len(msg_dec):
        if msg_dec[pos]==0:
            msg_dec.pop(pos)
        else:
            pos=pos+1
    print(msg_dec)
    msg_dec = str(msg_dec)
    msg_dec = msg_dec.translate({ord('['): None})
    msg_dec = msg_dec.translate({ord(']'): None})
    # Retorna dato desencriptado
    print("Datos enviados correctamente")
    return dict({'data': msg_dec})

@app.route('/datafile', methods=['GET'])
def requestDataFile():
    info = request.args.get("data").split(',')
    info[0] = info[0].split('.')[0]
    data = dict({
        'DateTime': info[0],
        'Sample': info[1]
    })
    data_id = db.child('Data').push(data)
    return dict({'status': True, 'id': data_id})

@app.route('/datafileid', methods=['GET'])
def requestDataF():
    data_id = request.args.get("id")
    data = db.child('Data').child(data_id).child('Sample').get().val()
    data = bytes.fromhex(data)
    # Desencripta el dato recibido
    decipher = AES.new(key.encode('utf-8'), AES.MODE_ECB)
    msg_dec = decipher.decrypt(data).hex()
    msg_dec = wrap(msg_dec, 4)
    for x in range(len(msg_dec)):
        msg_dec[x] = int(msg_dec[x], 16) * 3 / 4096
    pos = 0
    while pos < len(msg_dec):
        if msg_dec[pos] == 0:
            msg_dec.pop(pos)
        else:
            pos = pos + 1
    msg_dec = str(msg_dec)
    msg_dec = msg_dec.translate({ord('['): None})
    msg_dec = msg_dec.translate({ord(']'): None})
    # Retorna dato desencriptado
    print("Datos enviados correctamente")
    return dict({'data': msg_dec})

if __name__ == '__main__':
    app.run(debug=True)
