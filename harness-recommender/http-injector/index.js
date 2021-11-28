'use strict';

const crypto = require('crypto')
const fs = require('fs')
const path = require('path')
const loadtest = require('loadtest')

const ratings = [];
const csv = require('csv-parser');

var fluentd_host = process.env.FLUENTD_HOST
var async_fluentd = process.env.ASYNC_FLUENTD
var csvBody = process.env.CSV_BODY
var index = 0
var seed = process.env.SEED

var XP_NAME = process.env.XP_NAME
var xp_name = "{0}.{1}"
	.replace('{0}', 'private-recsys')
	.replace('{1}', XP_NAME)

// fluentd configuration
var logger = require('fluent-logger')
logger.configure(xp_name, {
	host: fluentd_host,
	port: 24224,
	timeout: 3.0,
	requireAckResponse: true,
	eventMode: 'CompressedPackedForward',
	reconnectInterval: 6000 // 1 minutes
});

logger.on('error', (error) => {
	console.log("logger error :", error);
});

logger.on('connect', () => {
	console.log('logger connected!');
});

process.on('SIGINT', function() {
	process.exit();
});

const sleep = (milliseconds) => {
	return new Promise(resolve => setTimeout(resolve, milliseconds))
}

function createMessage(userID) {
	let uid = encrypt(userID, P1pub)
	let randkey = crypto.randomBytes(16) // TODO
	let key = encrypt(randkey, P2pub)
	let body = {
		user: uid,
		key: key,
		timeSend: process.hrtime.bigint().toString()
	}
	const msg = JSON.stringify(body)
	const len = Buffer.byteLength(msg)
	const req = { message: msg, length: len }
	return req
}

var ratings_user = [];
fs.createReadStream(csvBody)
	.pipe(csv())
	.on('data', (data) => {
		if (typeof data.userId !== 'undefined') {
			ratings_user.push(createMessage(data.userId));
		}
	})
	.on('end', () => {
		console.log('CSV file successfully processed');
		loadtest.loadTest(options, function(error, result) {
			if (error) {
				return console.error('Got an error: %s', error)
			}
			console.log('Tests run successfully', result)

			if (async_fluentd == "true") {
				sleep(15000).then(() => {
					logger.end()
					console.log("End.")
					process.exit()
				})
			} else {
				if (fluentd_host != "") {
					console.log("Logging to fluentd on " + fluentd_host)
					// Save to fluentd
					for (let value of resultsArray) {
						//console.log(value)
						logger.emit('locust_request_result', value);
					}

				//logger.end()
					sleep(120000).then(() => {
						logger.end()
						console.log("End.")
						process.exit()
					})
		}

			}
		})
	});

// results array, sent to fluentd after experiment
var resultsArray = []

// RSA pub keys for proxies P1 and P2
const P1pub = fs.readFileSync(path.resolve('public_P1.pem'), 'utf8')
const P2pub = fs.readFileSync(path.resolve('public_P2.pem'), 'utf8') // TODO public_P2_1024.pem

function nextUser() {
	//let index = Math.floor(Math.random() * ratings_user.length)
	const buf = ratings_user[index]
	index = index + 1
	//console.log('ratings_user: ' + buf.toString());
	return buf
}

function encrypt(toEncrypt, key) {
	const buf = Buffer.from(toEncrypt, 'utf8')
	const encrypted = crypto.publicEncrypt({key:key,padding:crypto.constants.RSA_PKCS1_PADDING}, buf)
	return encrypted.toString('base64')
}

function requestGenerator(params, options, client, callback) {
	const user = nextUser()
	options.headers['Content-Length'] = user.length
	options.headers['Content-Type'] = 'application/json'
	const request = client(options, callback)
	request.write(user.message);
	return request
}

const options = {
	url: process.env.URL_BASE + "/engines/sgx_ur/queries",
	method: "POST",
	requestGenerator: requestGenerator,
	statusCallback: statusCallback,
	// Configuration options
	concurrency: process.env.CONCURRENCY,
	agentKeepAlive: true,
	requestsPerSecond: process.env.NB_REQUESTS,
	maxRequests: process.env.NB_REQUESTS * process.env.DURATION
	//maxSeconds: process.env.DURATION
}

function statusCallback(error, result, latency) {
	//console.log('result:', result);
	//console.log('Request elapsed milliseconds: ', result.requestElapsed);
	//console.log('Latency: ', latency);
	var entry = new Object()
	if (typeof result !== 'undefined') {
		entry.response_time = result.requestElapsed
		entry.response_length = result.headers["content-length"]
		entry.http_status = result.statusCode
		var content = result.body
		try {
			content = JSON.parse(content)
		} catch (e) {
			console.log(e)
		}
		entry.content = content
			entry.start = (new Date().getTime() - result.requestElapsed / 1000)
			entry.dt = new Date(entry.start).toISOString()

	}

	entry.exception = error

	entry.payload = ""
	entry.url = options.url
	if (async_fluentd == "true" && fluentd_host != "") {
		logger.emit('locust_request_result', entry);
	} else {
		resultsArray.push(entry)
	}
}

console.log("Lauching test on %s, rqs=%d duration=%d",
	process.env.URL_BASE + "/engines/sgx_ur/queries",
	process.env.NB_REQUESTS,
	process.env.DURATION
)
