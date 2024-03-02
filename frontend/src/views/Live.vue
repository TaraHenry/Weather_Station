<template>
    <v-container fluid class="bg-background" align="center">
        <!-- ROW 1 -->
        <v-row>
            <!-- COLUMN 1 -->
            <v-col cols="9">
                <figure class="highcharts-figure">
                    <div id="container"></div>
                </figure>
            </v-col>
            <!-- COLUMN 2 -->
            <v-col cols="3">
                <!-- CARD 1 -->
                <v-card class="mb-5" style="max-width: 500;" color="primaryContainer" subtitle="Temperature">
                    <v-card-item>
                        <span class="text-h3 text-onPrimaryContainer">{{ temperature }}</span>
                    </v-card-item>
                </v-card>
                <!-- CARD 2 -->
                <v-card class="mb-5" style="max-width: 500;" color="secondaryContainer" subtitle="Heat Index (Feels like)">
                    <v-card-item>
                        <span class="text-h3 text-onTertiaryContainer">{{ heatindex }}</span>
                    </v-card-item>
                </v-card>
                <!-- CARD 3 -->
                <v-card class="mb-5" style="max-width: 500;" color="primaryContainer" subtitle="Humidity">
                    <v-card-item>
                        <span class="text-h3 text-onSecondaryContainer">{{ humidity }}</span>
                    </v-card-item>
                </v-card>
            </v-col>
        </v-row>
        <!-- ROW 2 -->
        <v-row>
            <!-- COLUMN 1 -->
            <v-col cols="9">
                <figure class="highcharts-figure">
                    <div id="container1"></div>
                </figure>
            </v-col>
        </v-row>
    </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { onBeforeUnmount, onMounted, ref, watch } from "vue";
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

const points      = ref(10); // Specify the quantity of points to be shown on the live graph simultaneously.
const shift       = ref(false); // Delete a point from the left side and append a new point to the right side of the graph.

// COMPUTED PROPERTIES
// const temperature = computed(()=>{
//     if(!!payload.value){
//     return `${payload.value.temperature.toFixed(2)} °C`;
// }
// });

// const heatindex = computed(()=>{
//     if(!!payload.value){
//     return `${payload.value.heatindex.toFixed(2)} °C`;
// }
// });

// const humidity = computed(()=>{
//     if(!!payload.value){
//     return `${payload.value.humidity.toFixed(2)} °C`;
// }
// });

// FUNCTIONS

const CreateCharts = async () => {

    // TEMPERATURE CHART
    tempHiChart.value = Highcharts.chart('container', {
        chart: { zoomType: 'x' },
        title: { text: 'Temperature Analysis (Live)', align: 'left' },
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
};

// WATCHERS
watch(payload,(data)=> {
    if(points.value > 0){ points.value -- ; }
    else{ shift.value = true; }
    
    tempHiChart.value.series[0].addPoint({y:parseFloat(data.temperature.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);
    tempHiChart.value.series[1].addPoint({y:parseFloat(data.heatindex.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);
    humiChart.value.series[0].addPoint({y:parseFloat(data.humidity.toFixed(2)) ,x: data.timestamp * 1000 },
true, shift.value);
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
    border: 2px solid black;
}

</style>
  