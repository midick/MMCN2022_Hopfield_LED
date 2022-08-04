#include <stdlib.h> // for random number generator. maybe other stuff too idk
#include <stdio.h> // for debugging

// Train network with the hebbian rule. I guess maybe we can just do this beforehand and hardcode the matrix?
// change the [2][4] thing to [#patterns][#neurons] when we do it for real
void train_network(double weights[64*64], int num_neurons, double patterns[5][64], int num_patterns) {
    for(int i = 0; i < num_neurons; i++) {
        for(int j = 0; j < num_neurons; j++) {
            weights[i * num_neurons + j] = 0.0;
            for(int p = 0; p < num_patterns; p++){
                weights[i * num_neurons + j] += patterns[p][i] * patterns[p][j];
            }
            weights[i * num_neurons + j] /= ((double)num_patterns);
        }
    }
}


// Update a single neuron
void update_neuron(double weights[64*64], int num_neurons, double* current_pattern, int neuron) {
    double neuron_update = ((double)0.0);
    for(int i = 0; i < num_neurons; i++) {
        neuron_update += current_pattern[i] * weights[neuron + i * num_neurons];
    }
    current_pattern[neuron] = (neuron_update < ((double)0.0)) ? ((double)-1.0) : ((double)1.0);
}

// Random asynchronus update
void run_network_iterations(double weights[64*64], int num_neurons, double* current_pattern, int iterations) {
    for(int i = 0; i < iterations; i++) {
        for(int j = 0; j < num_neurons; j++) {
            // is there a random number issue with the chip thing?
            // can use random(0, num_neurons) on arduino i think
            int neuron = (int)(rand() / (((double)RAND_MAX + 1)/ ((double)num_neurons)));
            update_neuron(weights,num_neurons, current_pattern, neuron);
        }
    }
}



// Print weights
void show_network(double weights[64*64], int num_neurons) {
    printf("Weights:\n");
    for(int i = 0; i < num_neurons; i++) {
        for(int j = 0; j < num_neurons; j++) {
            printf("%f\t", weights[i * num_neurons + j]);
        }
        printf("\n");
    }
}


// Print neuron patterns as ones and zeros
void show_pattern(double pattern[64], int pattern_length) {
    for(int x = 0; x < pattern_length + 2; x++) {
        printf("-");
    }
    printf("\n");
    for(int y = 0; y < pattern_length; y++) {
        printf("|");
        for(int x = 0; x < pattern_length; x++) {
            printf("%c", pattern[x+pattern_length*y] > ((double)0.0) ? '.' : ' ');
        }
        printf("|\n");
    }
    
    for(int x = 0; x < pattern_length + 2; x++) {
        printf("-");
    }
    printf("\n");
}

double * generate_input(double patterns[5][64]) {
    static double current_pattern[64];


    //choose a pattern and add 30% noise
    int pattern_id =  (rand() % (5));
    for(int i = 0; i < 64; ++i){
        current_pattern[i] = patterns[pattern_id][i];
    }
    for(int i = 0; i < 64; ++i) {
        current_pattern[i] *= (rand()*1.0/RAND_MAX > ((double)0.2)) ? 1 : -1;
    }
    return current_pattern;

}




int main2(int argc, char** argv) {
    int num_neurons = 64;
    int num_patterns = 5;

    //initialize weights
    double weights[num_neurons*num_neurons];
    for(int i = 0; i < num_neurons; i++) {
        for(int j = 0; j < num_neurons; j++) {
            if(i == j) {
                weights[i * num_neurons + j] = ((double)0.0);
            }
            else {
                weights[i * num_neurons + j] = ((double)1.0) / (double)(num_neurons - 1);
            }
        }
    }


    //training patterns. replace 4 with 64 when we actually use it
    double patterns[5][64] = {
        //K
        {-1,-1,-1,-1,-1,-1,-1,-1, 
         -1,1,1,-1,-1,1,1,-1,
         -1,1,1,-1,1,1,-1,-1,
         -1,1,1,1,1,-1,-1,-1,
         -1,1,1,1,1,-1,-1,-1,
         -1,1,1,-1,1,1,-1,-1,
         -1,1,1,-1,-1,1,1,-1,
         -1,1,1,-1,-1,1,1,-1},
         //A
         {-1,-1,-1,-1,-1,-1,-1,-1, 
         -1,-1,-1,1,1,1,1,-1,
         -1,-1,1,1,-1,-1,1,1,
         -1,-1,1,1,-1,-1,1,1,
         -1,-1,1,1,1,1,1,1,
         -1,-1,1,1,-1,-1,1,1,
         -1,-1,1,1,-1,-1,1,1,
         -1,-1,1,1,-1,-1,1,1},
         //V
         {-1,-1,-1,-1,-1,-1,-1,-1, 
         -1,1,1,-1,-1,-1,1,1,
         -1,1,1,-1,-1,-1,1,1,
         -1,1,1,-1,-1,-1,1,1,
         -1,1,1,-1,-1,-1,1,1,
         -1,-1,1,1,-1,1,1,-1,
         -1,-1,-1,1,1,1,-1,-1,
         -1,-1,-1,-1,1,-1,-1,-1},
         //L
         {-1,-1,-1,-1,-1,-1,-1,-1, 
         1,1,-1,-1,-1,-1,-1,-1,
         1,1,-1,-1,-1,-1,-1,-1,
         1,1,-1,-1,-1,-1,-1,-1,
         1,1,-1,-1,-1,-1,-1,-1,
         1,1,-1,-1,-1,-1,-1,-1,
         1,1,1,1,1,-1,-1,-1,
         1,1,1,1,1,-1,-1,-1},
         //I
         {-1,-1,-1,-1,-1,-1,-1,-1, 
         -1,1,1,1,1,1,1,-1,
         -1,1,1,1,1,1,1,-1,
         -1,-1,-1,1,1,-1,-1,-1,
         -1,-1,-1,1,1,-1,-1,-1,
         -1,-1,-1,1,1,-1,-1,-1,
         -1,1,1,1,1,1,1,-1,
         -1,1,1,1,1,1,1,-1},
     };
    
    // train network
    train_network(weights, num_neurons, patterns, num_patterns);

    for(int num_trials = 0; num_trials < 20; ++num_trials){
        //give a randomish input pattern
        double * current_pattern = generate_input(patterns);

        /*
        //print training patterns
        printf("training patterns:\n");
        for(int i = 0; i < num_patterns; ++i) {
        show_pattern(patterns[i],  8);
        }
        */
        //print input pattern
        printf("input pattern\n");
        show_pattern(current_pattern,  8);
        

        // iterate
        for(int iterations = 0; iterations < 17; ++iterations){
            run_network_iterations(weights,  num_neurons, current_pattern, 1);
            
        }
        printf("output pattern\n");
        show_pattern(current_pattern,  8);
    }
    return 0;
}
