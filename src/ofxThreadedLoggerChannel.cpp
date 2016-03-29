//
//  ofxThreadedLoggerChannel.cpp
//  pattern
//
//  Created by Gal Sasson on 3/29/16.
//
//

#include "ofxThreadedLoggerChannel.h"

ofxThreadedLoggerChannel::ofxThreadedLoggerChannel(shared_ptr<ofBaseLoggerChannel> channel, bool bWaitForThreadOnExit)
{
	logChannel = channel;
	bWaitForThread = bWaitForThreadOnExit;
	bLoggerThreadRunning = true;
	loggerThread = std::thread(&ofxThreadedLoggerChannel::loggerThreadLoop, this);
}

ofxThreadedLoggerChannel::~ofxThreadedLoggerChannel()
{
	if (bWaitForThread) {
		bLoggerThreadRunning = false;
		if (loggerThread.joinable()) {
			loggerThread.join();
		}
	}
}

void ofxThreadedLoggerChannel::log(ofLogLevel level, const string &module, const string &message)
{
	logTasksLock.lock();
	logTasks.push_back(LogTask(level, module, message));
	logTasksLock.unlock();
}

void ofxThreadedLoggerChannel::log(ofLogLevel level, const string &module, const char *format, va_list args)
{
	log(level, module, ofVAArgsToString(format,args));
}

void ofxThreadedLoggerChannel::log(ofLogLevel level, const string &module, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log(level, module, format, args);
	va_end(args);
}

void ofxThreadedLoggerChannel::loggerThreadLoop()
{
	while (bLoggerThreadRunning) {
		logTasksLock.lock();
		vector<LogTask> todo = logTasks;
		logTasks.clear();
		logTasksLock.unlock();
		
		for (LogTask& task: todo) {
			_log(task);
		}
		
		ofSleepMillis(5);
	}
}

void ofxThreadedLoggerChannel::_log(const ofxThreadedLoggerChannel::LogTask &task)
{
	if (logChannel != NULL) {
		logChannel->log(task.level, task.module, task.message);
	}
}
