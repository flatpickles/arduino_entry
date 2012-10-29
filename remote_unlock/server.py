from flask import Flask, request, redirect
import twilio.twiml

app = Flask(__name__)

@app.route("/", methods=['GET', 'POST'])
def hello_monkey():
    txt = request.values.get('Body')

    resp = twilio.twiml.Response()
    if txt:
      resp.sms("You sent: " + txt)
    else:
      resp.sms("No message content received...")

    return str(resp)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=6288)