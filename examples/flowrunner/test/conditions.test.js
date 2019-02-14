/* eslint-env mocha */
/* global chai:false, conditionsFactory:false */

var expect = chai.expect;

describe('conditions', function() {

  it('totalTimeExceeds', function() {
    var results = [];
    results.push('<apex><general><duration>300</duration></general></apex>');
    results.push('<apex><general><duration>200</duration></general></apex>');    
    var conditions = conditionsFactory(results);
    
    expect(conditions.totalTimeExceeds(499)()).to.be.true;
    expect(conditions.totalTimeExceeds(500)()).to.be.false;
    expect(conditions.totalTimeExceeds(501)()).to.be.false;
  });

  it('timeTodayExceeds', function() {
    var longTimeAgo = '1999-01-01T00:00:00';
    var yesterday = new Date(Date.now() - 24*60*60*1000).toJSON();
    var now = new Date().toJSON();
    
    var results = [];
    results.push('<apex><general><enddate>' + longTimeAgo+ '</enddate><duration>100</duration></general></apex>');
    results.push('<apex><general><enddate>' + yesterday + '</enddate><duration>100</duration></general></apex>');    
    results.push('<apex><general><enddate>' + now + '</enddate><duration>100</duration></general></apex>');    
    var conditions = conditionsFactory(results);
    
    expect(conditions.timeTodayExceeds(99)()).to.be.true;
    expect(conditions.timeTodayExceeds(100)()).to.be.false;
  });

  it('timeLastExperimentExceeds', function() {
    var results = [];
    results.push('<apex><general><duration>100</duration></general></apex>');
    results.push('<apex><general><duration>300</duration></general></apex>');    
    results.push('<apex><general><duration>200</duration></general></apex>');    
    var conditions = conditionsFactory(results);
    
    expect(conditions.timeLastExperimentExceeds(199)()).to.be.true;
    expect(conditions.timeLastExperimentExceeds(200)()).to.be.false;
  });

  
});

