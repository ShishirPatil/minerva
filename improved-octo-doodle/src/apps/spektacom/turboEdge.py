import numpy as np
import pandas as pd
from dataFileTemplate import populateDataFileTemplate
import sys
import subprocess




class turboEdgeMaster:

    def __init__(self, model, path="./", outfile='models/model.h', makeFlash = "container-update/run.sh"):
        self.model = model
        self.nodeList = []
        self.dfolder = path
        self.outfile = outfile
        self.makeFlash = makeFlash
        self.sedcmd = "sed -i '/static.*/i __attribute__((section(\".container-data\")))'"
    
    def loadMatrices(self, folder):
        '''
        Loads Matrices B, W and Z from TLC format
        '''
        # W1 is stored as input dim * hidden dim
        df = pd.read_csv(folder + 'W1.csv', sep=',', header=None)
        W1 = np.matrix(df)
        w1_shape1 = W1.shape[1]

        # B1 is stored as hidden dim
        df = pd.read_csv(folder + 'B1.csv', sep=',', header=None)
        B1 = np.matrix(df)
        b1_shape0 = B1.shape[0]
        assert(w1_shape1 == b1_shape0)

        # W2 is stored as input dim * hidden dim
        df = pd.read_csv(folder + 'W2.csv', sep=',', header=None)
        W2 = np.matrix(df)
        w2_shape1 = W2.shape[1]

        # B2 is stored as hidden dim
        df = pd.read_csv(folder + 'B2.csv', sep=',', header=None)
        B2 = np.matrix(df)
        b2_shape0 = B2.shape[0]
        assert(w2_shape1 == b2_shape0)

        return W1, B1, W2, B2

    def createDataFile(self, W1, B1, W2, B2, outfile):
        '''
        Exports the provided matrices loaded, into a temp file
        so that they can be directly copied onto the MKR1000
        board's data.h file.
        '''
        valueDict = {}

        input_dim = W1.shape[0]
        hidden_dim = W1.shape[-1]
        valueDict['input_dim'] = input_dim
        valueDict['hidden_dim'] = hidden_dim
        output_dim = W2.shape[-1]
        valueDict['output_dim'] = output_dim

        W1Str = '\n\t\t'
        for i in range(0, W1.shape[0]):
            for j in range(0, W1.shape[1]):
                W1Str += str(W1[i, j]) + ','
            W1Str += '\n\t\t'
        valueDict['W1'] = W1Str[:-1]

        # assert(B.shape[0] == d_cap)

        B1Str = '\n\t\t'
        for i in range(0, B1.shape[0]):
            B1Str += '%f' % (B1[i]) + ','
        B1Str += '\n\t\t'
        valueDict['B1'] = B1Str[:-1]
        
        W2Str = '\n\t\t'
        for i in range(0, W2.shape[0]):
            for j in range(0, W2.shape[1]):
                W2Str += str(W2[i, j]) + ','
            W2Str += '\n\t\t'
        valueDict['W2'] = W2Str[:-1]

        # assert(B.shape[0] == d_cap)

        B2Str = '\n\t\t'
        for i in range(0, B2.shape[0]):
            B2Str += '%f' % (B2[i]) + ','
        B2Str += '\n\t\t'
        valueDict['B2'] = B2Str[:-1]

        # valueDict['prototypeLabelMatrix'] = ZStr[:-2]
        template = populateDataFileTemplate(valueDict)
        fin = open(outfile, 'w')
        fin.write(template)
        fin.close()
        return


    def save(self):
        '''
        Save weights as CSV. This is equivalent to ckpt!?
        '''
        weightsAndBiases = np.array(self.model.layers[0].get_weights())
        np.savetxt(self.dfolder+"W1.csv",weightsAndBiases[0] , delimiter=",")
        np.savetxt(self.dfolder+"B1.csv",weightsAndBiases[1] , delimiter=",")
        weightsAndBiases = np.array(self.model.layers[1].get_weights())
        np.savetxt(self.dfolder+"W2.csv",weightsAndBiases[0] , delimiter=",")
        np.savetxt(self.dfolder+"B2.csv",weightsAndBiases[1] , delimiter=",")
        print("Saved successfully")

    def export(self):
        '''
        Generate data.h file
        '''
        # Copy W1, B1, W2, B2
        W1, B1, W2, B2 = self.loadMatrices(self.dfolder)
        # Create a new file and export
        file = self.dfolder + self.outfile
        self.createDataFile(W1, B1, W2, B2, self.outfile)
        print("Created new model at", file)

    def redeploy(self):
        import difflib
        file = self.dfolder + self.outfile
        old_weights = file[:-2] + '_old.h'
        new_weights = file[:-2] + '_new.h'
        lines_of_diff = 0
        with open(old_weights, 'r') as hosts0:
            with open(new_weights, 'r') as hosts1:
                diff = difflib.unified_diff(
                    hosts0.readlines(),
                    hosts1.readlines(),
                    fromfile='hosts0',
                    tofile='hosts1',
                )
                for line in diff:
                    # sys.stdout.write(line)
                    lines_of_diff += 1
        if lines_of_diff == 0:
            print("No Difference, updating code container")
            return 0
        else:
            print("Updating weights container")
            return 1

    def deploy(self, deviceList = []):
        '''
        Write to device
        '''
        # Insert appropriate container attribute
        #if self.redeploy()==0:
        #    return 

        self.nodeList = deviceList
        sedcmd = self.sedcmd + " " + self.dfolder + self.outfile
        print("calling", sedcmd)
        subprocess.call([sedcmd], shell=True)
        # Make and send to device
        executer = self.dfolder + self.makeFlash
        subprocess.call([executer])
        return

    


                    



