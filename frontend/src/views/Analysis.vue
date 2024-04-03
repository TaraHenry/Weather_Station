<template>
  <!-- <div>
    <meta http-equiv="refresh" content="10"> 
  </div> -->
    <v-container fluid align="center" class="background" >
      <v-row class="row1" max-width="1200px" justify="center" align="center" padding="1">
          <v-col class="col col1" >
              <v-sheet class="pa-2 bg-background " height="250">
                  
                  <v-text-field label="Start date" type="Date" density="compact" solo-inverted class="mr-5" max-width="300px" flat v-model="start"></v-text-field>
                  <v-text-field label="End date" type="Date" density="compact" solo-inverted class="mr-5" max-width="300px" flat v-model="end"></v-text-field>
                  
                  <v-spacer></v-spacer>
                  <v-btn @click="updateLineCharts(); updateScatterCharts();" text="Analyze" color="primaryContainer" tonal></v-btn>
              </v-sheet> 
          </v-col>
      </v-row>
        <!-- <v-col cols="4" align="center">
          <v-card class="ma-2" title="Average Temperature" width="250" outlines color="tertiaryContainer" density="comfortable" rounded="lg" border subtitle="For the selected period">
            <v-card-item>
              <span class="text-h1 text-onTertiaryContainer">
                {{ celsTemperature.avg }}
                <span class="text-caption">Cels</span>
              </span>
            </v-card-item>
          </v-card>
        </v-col> -->
        <v-row style="max-width: 1200px;">
            <!-- COLUMN 1 -->
            <v-col cols="6">
                <figure class="highcharts-figure">
                    <div id="container"></div>
                </figure>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="6">
                <figure class="highcharts-figure">
                    <div id="container0"></div>
                </figure>
            </v-col>
        </v-row> 
        <v-row style="max-width: 1200px;">
            <!-- COLUMN 1 -->
            <v-col cols="6">
                <figure class="highcharts-figure">
                    <div id="container1"></div>
                </figure>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="6">
                <figure class="highcharts-figure">
                    <div id="container2"></div>
                </figure>
            </v-col>
            <!-- COLUMN 3 -->
            <v-col cols="12">
                <figure class="highcharts-figure">
                    <div id="container3"></div>
                </figure>
            </v-col>
        </v-row>
  </v-container>
</template>


<script setup>
/** JAVASCRIPT HERE */

import Highcharts from "highcharts";
import more from "highcharts/highcharts-more";
import Exporting from "highcharts/modules/exporting";
Exporting(Highcharts);
more(Highcharts);

// IMPORTS
import { onBeforeUnmount, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";
 
import { useAppStore } from "@/store/appStore";
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";


// VARIABLES
const Mqtt = useMqttStore();
const AppStore = useAppStore();
const router = useRouter();
const route = useRoute();
const { payload, payloadTopic } = storeToRefs(Mqtt);
var start = ref(null);
var end = ref(null);

const TempvsHumiLine = ref(null); // Chart object
const PressVsAltLine = ref(null); // Chart object
const SoilMoistureChart = ref(null);
const TempvsHILine   = ref(null);
const DHTvsBMPLine   = ref(null);


// FUNCTIONS

const CreateCharts = async () => {
  // TEMPERATURE CHART
  TempvsHumiLine.value = Highcharts.chart("container", {
    chart: { zoomType: "x" },
    title: { text: "Temperature vs. Humidity Analysis", align: "left" },
    yAxis: {
      title: {
        text: "Temperature",
        style: { color: "#000000" },
      },
      labels: { format: "{value} °C" },
    },

    tooltip: {
      pointFormat: "Temperature: {point.x} °C ",
    },
    xAxis: {
      title: { text: "Humidity", style: { color: "#000000" } },
      labels: { format: "{value} %" },
    },
    tooltip: { 
      shared:true,
      pointFormat: 'Humidity: {point.x} % <br/> Temperature: {point.y} °C' 
    }, 
    series: [
      {
        name: "Analysis",
        type: "line",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[2],
      },
      // {
      //   name: "Humidity",
      //   type: "line",
      //   data: [],
      //   turboThreshold: 0,
      //   color: Highcharts.getOptions().colors[6],
      // },    
    ],
  });

  PressVsAltLine.value = Highcharts.chart("container2", {
    chart: { zoomType: "x" },
    title: {
      text: "Pressure and Altitude Correlation Analysis",
      align: "left",
    },
    yAxis: {
      title: {
        text: "Pressure",
        style: { color: "#000000" },
      },
      labels: { format: "{value} Pa" },
    },

    xAxis: {
      title: { text: "Altitude", style: { color: "#000000" } },
      labels: { format: "{value} m" },
    },
    tooltip: {
      shared:true,
      pointFormat: 'Altitude: {point.x} m <br/> Pressure: {point.y} Pa' 
    },
    series: [
      {
        name: "Analysis",
        type: "spline",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[2],
      },
    ],
  });
  
  DHTvsBMPLine.value = Highcharts.chart("container1", {
    chart: { zoomType: "x" },
    title: { text: " DHT Temperature and BMP Temperature Comparison", align: "left" },
    yAxis: {
      title: {
        text: "DHT Temperature",
        style: { color: "#000000" },
      },
      labels: { format: "{value} °C" },
    },
    xAxis: {
      title: { text: "BMP Temperature", style: { color: "#000000" } },
      labels: { format: "{value} °C" },
    },
    tooltip: { 
      shared:true,
      pointFormat: 'BMP Temperature: {point.x} °C <br/> DHT Temperature: {point.y} °C',
    },
    series: [
      {
        name: "Analysis",
        type: "spline",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[6],
      },
      // {
      //   name: "BMP",
      //   type: "line",
      //   data: [],
      //   turboThreshold: 0,
      //   color: Highcharts.getOptions().colors[2],
      // },    
    ],
  });

  TempvsHILine.value = Highcharts.chart("container0", {
    chart: { zoomType: 'x' },
        title: { text: 'Temperature & Heat Index Analysis', align: 'left' },
        subtitle: { text: 'The heat index, also known as the "apparent temperature," is a measure that combines air temperature and \
        relative humidity to assess how hot it feels to the human body. The relationship between heat index and air temperature is \
        influenced by humidity levels. As humidity increases, the heat  index also rises, making the perceived temperature higher \
        than the actual air temperature.', align: 'left'},
        yAxis: {
            title: { text: 'Temperature' , style:{color:'#000000'}},
            labels: { format: '{value} °C' }
        },
        xAxis: {
            type: 'datetime',
            title: { text: 'Heat Index', style:{color:'#000000'} },
            labels: { format: '{value} °F' }
        },
        tooltip: { 
            shared:true,
            pointFormat: 'Heat Index: {point.x} °F <br/> Temperature: {point.y} °C' },
        series: [
        {
            name: 'Analysis',
            type: 'spline',
            data: [],
            turboThreshold: 0,
            color: Highcharts.getOptions().colors[0]
        },
        // {
        //     name: 'Heat Index',
        //     type: 'spline',
        //     data: [],
        //     turboThreshold: 0,
        //     color: Highcharts.getOptions().colors[1]
        // } 
      ],
    });

  SoilMoistureChart.value = Highcharts.chart('container3', {
        chart: { zoomType: 'x' },
        title: { text: 'Soil Moisture Analysis', align: 'left' },
        yAxis: {
            labels: { format: '{value} %' }
        },
        xAxis: {
            type: 'datetime',
            title: { text: 'Time', style:{color:'#000000'} },
        },
        tooltip: { shared:true },
        series: [
            {
                name: 'Soil Moisture',
                type: 'scatter',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[0]
            }
        ],
    });
};

onMounted(() => {
  // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
  Mqtt.connect(); // Connect to Broker located on the backend
  setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620154033");
    Mqtt.subscribe("620154033_sub");
  }, 3000);
  CreateCharts();
});

onBeforeUnmount(() => {
  // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
  Mqtt.unsubcribeAll();
});

const updateLineCharts = async () => {
    if (!!start.value && !!end.value) {
      // Convert output from Textfield components to 10 digit timestamps
      let startDate = new Date(start.value).getTime() / 1000;
      let endDate = new Date(end.value).getTime() / 1000;
      // Fetch data from backend
      const data = await AppStore.getAll(startDate, endDate);
      // Create arrays for each plot
      let dhtvsbmp = [];
      let tempvshI = [];
      let tempvsHum = [];
      let pressvsAlt = [];

        // Iterate through data variable and transform object to format recognized by highcharts
        data.forEach(row => {
            tempvsHum.push({"x": parseFloat(row.humidity.toFixed(2)), "y": parseFloat(row.celsTemperature.toFixed(2)) });
            tempvshI.push({"x": parseFloat(row.heatindex.toFixed(2)), "y": parseFloat(row.celsTemperature.toFixed(2)) });
            dhtvsbmp.push({"x": parseFloat(row.bmp_temp.toFixed(2)), "y": parseFloat(row.celsTemperature.toFixed(2)) });
            pressvsAlt.push({"x": parseFloat(row.altitude.toFixed(2)), "y": parseFloat(row.pressure.toFixed(2)) });
        });

   
      // Add data to Temperature and Heat Index chart
      TempvsHILine.value.series[0].setData(tempvshI);
      // Add data to Temperature and Humidity chart
      TempvsHumiLine.value.series[0].setData(tempvsHum);
      // Add data to DHT and BMP chart
      DHTvsBMPLine.value.series[0].setData(dhtvsbmp);
      // Add data to Pressure and Altitude chart
      PressVsAltLine.value.series[0].setData(pressvsAlt);
    }
  }; 

const updateScatterCharts = async ()=>{
    if(!!start.value && !!end.value){
        // Convert output from Textfield components to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;
        
        // Fetch data from backend
        const data = await AppStore.getAll(startDate,endDate);

        // Create arrays for each plot
        let s1 = [];

        // Iterate through data variable and transform object to format recognized by highcharts
        data.forEach(row => {
            s1.push({ "x": row.timestamp * 1000 , "y": parseFloat(row.soilMoisture.toFixed(2)) });
        });

        // Add data to Soil Moisture Analysis chart
        SoilMoistureChart.value.series[0].setData(s1);
    }
}
</script>

<style scoped>
/** CSS STYLE HERE */

.container {
  background-color: #f5f5f5;
  width: 1200px;
}

.row {
  max-width: 1200px;
}

.row1 {
  max-width: 1200px;
  padding: 1;
}

.col1 {
  border: black;
}

.sheet {
  padding: 2;
  height: 250;
}

Figure {
  border: 2px solid pink;
}
</style> 