<template>
    <v-container class="container" justify="center" align="center">
        <v-row class="row1" justify="center" >
            <v-card title="Combination" color="background" subtitle="Enter your four digit passcode" flat>
                <v-otp-input focus-all :length="4" variant="solo-filled" color="primary" v-model="passcode"></v-otp-input>
            </v-card> 
        </v-row>
        <v-row class="row2" justify="center">
            <v-card class="bg-background" flat>
                <v-btn class="bg-primary text-button" text="Submit" variant="tonal" @click="storePasscode();"></v-btn>
            </v-card>
        </v-row>
    </v-container>
</template>

<script setup>

import { computed, onBeforeUnmount, onMounted, ref, watch, reactive } from "vue";
import { useRoute, useRouter } from "vue-router";
import { useAppStore } from "@/store/appStore";
import { useMqttStore } from "@/store/mqttStore"; // Import Mqtt Store
import { storeToRefs } from "pinia";
// VARIABLES
const router      = useRouter();
const route       = useRoute();  
const AppStore    = useAppStore();
const Mqtt        = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);

const passcode = ref(["0000"]);

// FUNCTIONS
onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout( ()=>{
        // Subscribe to each topic
        Mqtt.subscribe("620154033");
        Mqtt.subscribe("620154033_pub");
    },3000)
});

onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    Mqtt.unsubcribeAll();
});

function storePasscode() {
    // Code to read passcode here
    console.log(passcode.value);
    AppStore.getSetPwd(passcode.value);
    
}
</script>


<style scoped>
/** CSS STYLE HERE */
.container{
    /* background-color: purple; */
}

.row1{
    /* background-color: brown; */
    max-width: 1200px;
    
}


.col1{
    /* background-color: blueviolet; */
}
</style>
  