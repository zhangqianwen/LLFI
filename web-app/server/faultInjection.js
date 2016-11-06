var fs = require('fs');
var readline = require('readline');
var exec = require('child_process').exec;
var LLFI_BUILD_ROOT = "./../../../../installer/llfi/";

exports.processFaultInjection = function (req, res) {

	var fileName = req.body.fileName;
	// Remove the file extension
	fileName = fileName.replace(/\.[^/.]+$/, "");
	var input = req.body.input;
	var batchMode = req.body.injectionMode.isBatchMode;
	var faultInjectionScript;
	if (batchMode) {
		faultInjectionScript = LLFI_BUILD_ROOT + "bin/batchInjectfault " + fileName + ".ll " + input;
	} else {
		faultInjectionScript = LLFI_BUILD_ROOT + "bin/injectfault " + "./llfi/" + fileName + "-faultinjection.exe " + input;
	}

	var cdDirCmd = "cd ./uploads/" + req.ip +"/";
	var consoleLog = [];
	var commands = [];
	commands.push(cdDirCmd + " && " + faultInjectionScript);

	commands.reduce(function(p, cmd) {
		return p.then(function(results) {
			return execPromise(cmd).then(function(stdout) {
				results.push(stdout);
				consoleLog = results;
				return results;
			}, function(err) {
				console.log("fault injection err: ", err);
			});
		});
	}, Promise.resolve([])).then(function(results) {
		console.log("faultInjection success");
		var results = {consoleLog: consoleLog};
		res.send(results);
	});
}

var execPromise = function(cmd) {
	return new Promise(function(resolve, reject) {
		exec(cmd, function(err, stdout) {
			if (err) return reject(err);
			resolve(cmd + stdout);
		});
	});
}