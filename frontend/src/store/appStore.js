import { defineStore } from 'pinia';


export const useAppStore =  defineStore('app', ()=>{

    /*  
    The composition API way of defining a Pinia store
    ref() s become state properties
    computed() s become getters
    function() s become actions  
    */ 

    // STATES 

    const getAll = async (start, end) => {
        // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
        const controller = new AbortController();
        const signal = controller.signal;
        const id = setTimeout(() => { controller.abort() }, 60000);
        const URL = `/api/weather/get/${start}/${end}`;
        try {
            const response = await fetch(URL, { method: 'GET', signal: signal });
            if (response.ok) {
                const data = await response.json();
                // console.log(data);
                
                let keys = Object.keys(data);
                if (keys.includes("status")) {
                    if (data["status"] == "found") {
                        // console.log(data["data"]);
                        return data["data"];
                    }
                    if (data["status"] == "failed") {
                        console.log("getAll returned no data");
                    }
                }
            }
            else {
                const data = await response.text();
                console.log(data);
            }
        }
        catch (err) {
  
            console.error('getAll in appstore error:', err.message);
        }
        return []
    } 


    // const get_calculate_avg = async (start, end) => {
    //     // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
    //     const controller = new AbortController();
    //     const signal = controller.signal;
    //     const id = setTimeout(() => { controller.abort() }, 60000);
    //     const URL = `/api/avg/${start}/${end}`;
    //     try {
    //         const response = await fetch(URL, { method: 'GET', signal: signal });
    //         if (response.ok) {
    //             const data = await response.json();
    //             let keys = Object.keys(data);
    //             if (keys.includes("status")) {
    //                 if (data["status"] == "complete") {
    //                     console.log(data["data"]);
    //                     return data["data"];
    //                 }
    //                 if (data["status"] == "failed"
    //                 ) {
    //                     console.log("get_calculate_avg returned no data");
    //                 }
    //             }
    //         }
    //         else {
    //             const data = await response.text();
    //             console.log(data);
    //         }
    //     }
    //     catch (err) {

    //         console.error('get_calculate_avg error:', err.message);
    //     }
    //     return []
    // }
    
   const getUpdateData= async (data) => {
       const controller = new AbortController();
       const signal = controller.signal;
       const id= setTimeout(() => controller.abort(), 60000);
       const URL= '/api/update';

         try {
                const response = await fetch(URL, {
                    method: 'POST',
                    signal: signal,
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify(data)
                });
                const result = await response.json();
                console.log(result);
                clearTimeout(id);
                return result;
            } catch (error) {
                console.log(error);
                clearTimeout(id);
                return {status: 'getUpdatedata error', message: error.message};
            }
            return [];
    }
  
   

    // ACTIONS
 
   
 
 
    return { 
    // EXPORTS	
    // get_calculate_avg,
    getAll,
    getUpdateData,
            
       }
},{ persist: true  });