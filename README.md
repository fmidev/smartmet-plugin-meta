Table of Contents
=================

  * [SmartMet Server](#SmartMet Server)
  * [Introduction](#Introduction)
    * [Example: Quality code request](#example-quality-code-request)
    * [Example: observableProperty](#example-observableproperty)

# SmartMet Server

[SmartMet Server](https://github.com/fmidev/smartmet-server) is a data
and product server for MetOcean data. It provides a high capacity and
high availability data and product server for MetOcean data. The
server is written in C++, since 2008 it has been in operational use by
the Finnish Meteorological Institute FMI.

# Introduction 

The purpose of the meta plugin is to provide access to the meta data of observations.
We can specify the language in which we would like to have the the metadata in the request. 
Currently the plugin supports both  Finnish (fin) and English (eng) languages.


For the meta plugin  the following access parameters are available:

* qualityCode
* observableProperty



## Example: Quality code request

There are 10 quality codes which are the following:

| quality code | description|
|--------------|------------|
|0             | Unknown quality |
|1             | Good quality (automatically checked) |
|2             | Good quality (manually checked) |
|3             | Accepted quality (automatically checked |
|4             | Accepted  quality (manually checked) |
|5             | Missing value in database |
|6             | Not accepted quality (automatically checked) |
|7             | Not accepted  quality (manually checked) |
|8             | Erroneous value (automatically checked |
|9             | Erroneous value (manually checked |


Here is an example showing the quality code request.

<pre><code>
http://data.fmi.fi/meta?qualitycode=0&language=eng&fmi-apikey=YOUR-FMI-APIKEY&
</code></pre>

The response to the meta data request for the quality code 0 is given below. 
````
<Category xsi:schemaLocation="http://www.opengis.net/swe/2.0 http://schemas.opengis.net/sweCommon/2.0/swe.xsd">    
  -<quality>
   -<Category id="qc-0">
     <label>Unknown quality</label>
     -<description>
          Value is not quality controlled and it may be erroneous. This quality code is typically given for observations that are not checked in any way. For example foreign synops and road weather observations.
     </description>
     <value>0</value>
   </Category>
  </quality>
</Category>
````


## Example: observableProperty

In the example below shows the metadata request for the observableProperty, "Air temperature".

<pre><code>
http://data.fmi.fi/fmi-apikey/YOUR-FMI-APIKEY/meta?observableProperty=observation&param=TA&language=eng
</code></pre>


The response to the meta data request for the observableProperty, "Air temperature" is given below. 

````
<ObservableProperty gml:id="ta" xsi:schemaLocation="http://inspire.ec.europa.eu/schemas/omop/2.9 http://inspire.ec.europa.eu/draft-schemas/omop/2.9
/ObservableProperties.xsd">
   <label>Air temperature</label>
   <basePhenomenon>Temperature</basePhenomenon>
   <uom uom="degC"/>
   -<statisticalMeasure>
      -<StatisticalMeasure gml:id="stat-avg-PT1M-ta---">
           <statisticalFunction>avg</statisticalFunction>
           <aggregationTimePeriod>PT1M</aggregationTimePeriod>
       </StatisticalMeasure>
  </statisticalMeasure>
</ObservableProperty>

````

