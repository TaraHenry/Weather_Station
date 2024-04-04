<template>
    <div>
    <meta http-equiv="refresh" content="10"> 
  </div>
    <v-container fluid class="bg-background" align="center">
        <!-- ROW 1 -->
        <v-col justify="center" style="max-width: 1200px;">
            <v-card class="text-onPrimary" color="primaryContainer" style="max-width: 1200px; max-height: 800px;" flat>
                <h1>Temperature and Heat Index</h1>
            </v-card>
        </v-col>
        <v-row>
            <!-- COLUMN 1 -->
            <v-col cols="9">
                <figure class="highcharts-figure">
                    <div id="container"></div>
                </figure>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="3">
                <v-card class="mb-5" style="max-width: 500;" color="primaryContainer" subtitle=" Celsius Temperature">
                    <v-card-item>
                        <span class="text-h2 text-onPrimaryContainer">{{ temperature }}</span>
                        <v-btn @click="Tconvert();" text="Convert" color="text-onPrimaryContainer" variant="tonal"></v-btn>
                    </v-card-item>
                </v-card>
                <!-- <v-card class="mb-5" style="max-width: 500;" color="secondaryContainer" subtitle=" Fahrenheit Temperature">
                    <v-card-item>
                        <span class="text-h3 text-onSecondaryContainer">{{ fahrTemperature }}</span>
                    </v-card-item>
                </v-card> -->
                <v-card class="mb-5" style="max-width: 500;" color="background" subtitle="Heat Index (Feels like)">
                    <v-card-item>
                        <span class="text-h2 text-onSecondaryContainer">{{ heatindex }}</span>
                    </v-card-item>
                </v-card>
            </v-col>
        </v-row>
        <!-- ROW 2 -->
        <v-col justify="center" style="max-width: 1200px;">
            <v-card class="mt-5 text-onPrimary" color="primaryContainer" style="max-width: 1200px; max-height: 800px;" flat>
                <h1>Humidity</h1>
            </v-card>
        </v-col>
        <v-row>
            <!-- COLUMN 1 -->
            <v-col cols="9">
                <figure class="highcharts-figure" >
                    <div id="container1"></div>
                </figure>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="3">
                <!-- CARD 3 -->
                <v-card color="primaryContainer" >
                    <v-card-item>
                        <span class="text-h2 text-onPrimaryContainer">{{ humidity }}</span>
                    </v-card-item>
                </v-card>
            </v-col>
        </v-row>
        <!-- ROW 3 -->
        <v-row>
            <v-col cols="6">
                <v-card class="mt-5 text-onPrimary" color="primaryContainer" style="max-width: 800px; max-height: 800px;" flat>                    <h1>Pressure</h1>
                </v-card>
                <v-card class="mt-4" style="max-width: 800;" color="tertiaryContainer">
                    <v-card-item>
                        <span class="text-h2 text-onTertiaryContainer"> {{ pressure }}</span>
                    </v-card-item>
                </v-card>
            </v-col> 
            <v-col cols="6">
                <v-card class="mt-5 text-onPrimary" color="primaryContainer" style="max-width: 800px; max-height: 800px;" flat>
                    <h1>Approximate Altitude</h1>
                </v-card>
                <v-card class="mt-4" style="max-width: 800;" color="tertiaryContainer">
                    <v-card-item>
                        <span class="text-h2 text-onTertiaryContainer">{{ altitude }}</span>
                    </v-card-item>
                </v-card>
            </v-col>
        </v-row>
        <!-- ROW 4 -->
        <v-col justify="center" style="max-width: 1200px;">
            <v-card class="mt-5 text-onPrimary" color="primaryContainer" style="max-width: 1200px; max-height: 800px;" flat>
            <h1>Soil Moisture</h1>
          </v-card>
        </v-col>
        <v-row>
            <v-col cols="9">
                <figure class="highcharts-figure">
                    <div id="container2"></div>
                </figure>
            </v-col>
            <v-col cols="3">
                <v-card class="mb-5" style="max-width: 500;" color="background">
                    <v-card-item>
                        <v-sheet color="background" class="mt-5" width="200">
                            <span class="text-h2 text-onSecondaryContainer">{{ soilMoisture }}</span>
                            <v-slider color="tertiaryContainer" v-model="soilMoisture" max="100%" thumb-label direction="vertical" track-size="60"></v-slider>
                        </v-sheet>
                    </v-card-item>
                </v-card>
            </v-col>
        </v-row>
    </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { computed, onBeforeUnmount, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";
 
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";

// Highcharts, Load the exporting module and Initialize exporting module.
import Highcharts from 'highcharts';
import more from 'highcharts/highcharts-more';
import Exporting from 'highcharts/modules/exporting';
Exporting(Highcharts);
more(Highcharts);

// VARIABLES
const router      = useRouter();
const route       = useRoute();  

const Mqtt        = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);

const tempHiChart = ref(null); // Chart object
const humiChart   = ref(null); // Chart object
const soilChart   = ref(null);

// const currentPUnit = ref("hPa");
// const currentTUnit = ref("C");
// const currentAUnit = ref("m");

// const pressure    = ref({value: null, unit: "Pa"});
// const altitude    = ref({value: null, unit: "m"});

const points      = ref(10); // Specify the quantity of points to be shown on the live graph simultaneously.
const shift       = ref(false); // Delete a point from the left side and append a new point to the right side of the graph.

var Tstate = 0;
// COMPUTED PROPERTIES

const temperature = computed(()=>{
    if(!!payload.value){
        if(Tstate == 1) {
            return `${payload.value.fahrTemperature.toFixed(2)} °F`;
        }else{
            return `${payload.value.celsTemperature.toFixed(2)} °C`;
        }
    }
});

const heatindex = computed(()=>{
    if(!!payload.value){
    return `${payload.value.heatindex.toFixed(2)} °F`;
}
});

const humidity = computed(()=>{
    if(!!payload.value){
    return `${payload.value.humidity.toFixed(2)} %`;
}
});

const soilMoisture = computed(()=>{
    if(!!payload.value){
    return `${payload.value.soilMoisture.toFixed(2)} %`;
}
});

const pressure = computed(()=>{
    if(!!payload.value){
        if(Pstate == 1) {
            const value = `${payload.value.pressure.toFixed(2)} kPa`;
            return value/1000;
            `${payload.value.pressure.toFixed(2)} kPa`;
        }else{
            return `${payload.value.pressure.toFixed(2)} Pa`;
        }
    }
});

const altitude = computed(()=>{
    if(!!payload.value){
        if(Astate == 1) {
            const value = `${payload.value.altitude.toFixed(2)} cm`;
            return value * 100;
        }else{
            return `${payload.value.altitude.toFixed(2)} m`;
        }
    }
});


// FUNCTIONS
const Tconvert = async () => {
    if(Tstate == 0){
        Tstate = 1;
    }
    else{
        Tstate = 0;
    }
};

const CreateCharts = async () => {

    // TEMPERATURE CHART
    tempHiChart.value = Highcharts.chart('container', {
        chart: { zoomType: 'x' },
        title: { text: 'Temperature Analysis (Live)', align: 'left' },
        yAxis: {
            labels: { format: '{value} °C' }
        },
        xAxis: {
            type: 'datetime',
            title: { text: 'Time', style:{color:'#000000'} },
        },
        tooltip: { shared:true },
        series: [
            {
                name: 'Temperature',
                type: 'spline',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[0]
            },
            {
                name: 'Heat Index',
                type: 'spline',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[1]
            } ],
    });
    // HUMIDITY CHART
    humiChart.value = Highcharts.chart('container1', {
        chart: { zoomType: 'x' },
        title: { text: 'Humidity Analysis (Live)', align: 'left' },
        yAxis: {
            //title: { text: 'Air Temperature & Heat Index' , style:{color:'#000000'}},
            labels: { format: '{value} °C' }
        },
        xAxis: {
            type: 'datetime',
            title: { text: 'Time', style:{color:'#000000'} },
        },
        tooltip: { shared:true, },
        series: [
            {
                name: 'Humidity',
                type: 'spline',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[1]
            } ],
    });
    // SOIL MOISTURE CHART
    soilChart.value = Highcharts.chart('container2', {
        chart: { zoomType: 'x' },
        title: { text: 'Soil Moisture Analysis (Live)', align: 'left' },
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
                type: 'spline',
                data: [],
                turboThreshold: 0,
                color: Highcharts.getOptions().colors[0]
            }
        ],
    });
};

// WATCHERS
watch(payload,(data)=> {
    if(points.value > 0){ points.value -- ; }
    else{ shift.value = true; }
    
    tempHiChart.value.series[0].addPoint({y:parseFloat(data.celsTemperature.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);
    tempHiChart.value.series[1].addPoint({y:parseFloat(data.heatindex.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);
    humiChart.value.series[0].addPoint({y:parseFloat(data.humidity.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);
    soilChart.value.series[0].addPoint({y:parseFloat(data.soilMoisture.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);

    // pressure.value = data?.pressure;
    // altitude.value = { value: data?.altitude, unit: data?.altitude?.unit };
    // console.log("Updated altitude:", data?.altitude);
})

onMounted(()=>{
// THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    CreateCharts();
    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout( ()=>{
        // Subscribe to each topic
        Mqtt.subscribe("620154033");
        Mqtt.subscribe("620154033_pub");
    },3000)
});

onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    // unsubscribe from all topics
    Mqtt.unsubcribeAll();
});

</script>


<style scoped>
/** CSS STYLE HERE */
Figure {
    border: 2px solid pink;
}

</style>
  