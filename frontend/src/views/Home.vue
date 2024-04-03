<template>
  
  <VContainer class="fill-height ">
    <VResponsive class="align-center text-center fill-height">
      
      <div class="text-body-2 font-weight-light mb-4">Welcome to</div>

      <h1 class="text-h2 font-weight-bold">My Weather Station</h1>
      <div class="text-body-1 font-weight-bold mt-5">Creator: Tara Henry</div>
 
      <div class="py-8"/>
      <VImg height="500" class="mb-5" src="@/assets/logo.svg" />
    </VResponsive>
  </VContainer>

</template>

<script setup>
  import { storeToRefs } from 'pinia';
  import { useMqttStore } from '@/store/mqttStore';
  import { ref,reactive, watch, onMounted, onBeforeUnmount } from 'vue';


// VARIABLES


// FUNCTIONS
onMounted(()=>{
   
});

onBeforeUnmount(()=>{ 
  
});
</script>
