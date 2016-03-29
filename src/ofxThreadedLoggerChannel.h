//
//  ofxThreadedLoggerChannel.h
//  pattern
//
//  Created by Gal Sasson on 3/29/16.
//
//

#ifndef ofxThreadedLoggerChannel_h
#define ofxThreadedLoggerChannel_h

#include <stdio.h>
#include "ofMain.h"

class ofxThreadedLoggerChannel : public ofBaseLoggerChannel
{
public:
	ofxThreadedLoggerChannel(shared_ptr<ofBaseLoggerChannel> channel, bool bWaitForThreadOnExit=true);
	~ofxThreadedLoggerChannel();
	void log(ofLogLevel level, const string & module, const string & message);
	void log(ofLogLevel level, const string & module, const char* format, va_list args);
	void log(ofLogLevel level, const string & module, const char* format, ...)  OF_PRINTF_ATTR(4, 5);

private:
	class LogTask {
	public:
		LogTask(ofLogLevel l, const string& mod, const string& msg) : level(l), module(mod), message(msg) {}
		ofLogLevel level;
		string module;
		string message;
	};
	vector<LogTask> logTasks;
	std::mutex logTasksLock;
	std::thread loggerThread;
	bool bLoggerThreadRunning;
	shared_ptr<ofBaseLoggerChannel> logChannel;
	bool bWaitForThread;
	
	void loggerThreadLoop();
	void _log(const LogTask& task);
};

#endif /* ofxThreadedLoggerChannel_h */
