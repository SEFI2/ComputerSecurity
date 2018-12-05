#!/usr/bin/env python
from flask_cors import CORS, cross_origin
import flask
app  = flask.Flask(__name__)
#cors = CORS(app)
#app.config['CORS_HEADERS'] = 'Content-Type'


'''
The attacker's server for lab2.
'''
main_page = '''
<!doctype html><html>Working!</html>
'''
empty_page = '''
<!doctype html>
<html lang="en">
Message received!
</html>
'''

@app.route("/",methods=['GET'])
#@cross_origin()
def resp_main_page():
    print('DATA: ',flask.request.data)
    print(main_page)
    resp = flask.Response(main_page)
    print(resp)
    return resp

@app.route("/benign",methods=['POST'])
#@cross_origin()
def benign():
    print('POST_DATA: ', flask.request.args)
    print('POST_FORM: ', flask.request.form)
    resp = flask.Response(empty_page)
    return resp



if __name__ == '__main__':
    app.run(host='example.org', port=4444)
