//
//  FirstDerivative.cpp
//  epanet-rtx
//
//  Created by the EPANET-RTX Development Team
//  See README.md and license.txt for more information
//  

#include "FirstDerivative.h"

using namespace std;
using namespace RTX;

FirstDerivative::FirstDerivative() {
  
}

FirstDerivative::~FirstDerivative() {
  
}


TimeSeries::PointCollection FirstDerivative::filterPointsInRange(TimeRange range) {
  PointCollection data(vector<Point>(), this->units());
  
  vector<Point> outPoints;
  Units fromUnits = this->source()->units();
  
  // left difference, so get one more to the left.
  Point leftMostPoint = this->source()->pointAtOrBefore(range.first);
  Point prior = this->source()->pointBefore(leftMostPoint.time);
  
  // may not have a point here. if not, get the next one.
  if (prior.time == 0) {
    prior = this->source()->pointAfter(range.first - 1);
  }
  
  // get next point in case it's out of the specified range
  Point seekRight;
  seekRight.time = range.second - 1;
  while (seekRight.time > 0 && !seekRight.isValid) {
    seekRight = this->source()->pointAfter(seekRight.time);
  }
  if (seekRight.time > 0) {
    range.second = seekRight.time;
  }
  
  PointCollection sourceData = this->source()->points(make_pair(prior.time, range.second));
  
  if (sourceData.count() < 2) {
    return data;
  }
  
  vector<Point>::const_iterator cursor, prev, vEnd;
  cursor = sourceData.points.begin();
  prev = sourceData.points.begin();
  vEnd = sourceData.points.end();
  
  ++cursor;
  while (cursor != vEnd) {
    time_t dt = cursor->time - prev->time;
    double dv = cursor->value - prev->value;
    double dvdt = Units::convertValue(dv / double(dt), fromUnits / RTX_SECOND, this->units());
    outPoints.push_back(Point(cursor->time, dvdt));
    ++cursor;
    ++prev;
  }
  
  
  data.points = outPoints;
  
  if (this->willResample()) {
    set<time_t> timeValues = this->timeValuesInRange(range);
    data.resample(timeValues);
  }
  
  return data;
}

bool FirstDerivative::canSetSource(TimeSeries::_sp ts) {
  return (!this->source() || this->units().isSameDimensionAs(ts->units() / RTX_SECOND));
}

void FirstDerivative::didSetSource(TimeSeries::_sp ts) {
  if (this->units().isDimensionless() || !this->units().isSameDimensionAs(ts->units() / RTX_SECOND)) {
    Units newUnits = ts->units() / RTX_SECOND;
    if (newUnits.isDimensionless()) {
      newUnits = RTX_DIMENSIONLESS; // fix weird hr/sec units bug
    }
    this->setUnits(newUnits);
  }
}

bool FirstDerivative::canChangeToUnits(Units units) {
  if (!this->source()) {
    return true;
  }
  else if (units.isSameDimensionAs(this->source()->units() / RTX_SECOND)) {
    return true;
  }
  else {
    return false;
  }
}



std::ostream& FirstDerivative::toStream(std::ostream &stream) {
  TimeSeries::toStream(stream);
  if (source()) {
    stream << "First Derivative Of: " << *source() << "\n";
  }
  
  return stream;
}
