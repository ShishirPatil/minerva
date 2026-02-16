
Timer interrupt goes off every 6 hours -> interrupt handler checks for new code
User gets access to a global symbol `CAN_UPDATE` -> if it is low then interrupt handler exits immediately
ML code is in a seperate memory region

Change in philosophy, updates occur very rarely. We justify a paradighm where we need to update more frequently (6 hours). It doesn't make sense to flash the whole update, but rather only the necessary portion of the update. ML and federated learning is the motivation. We benchmark downtime, footprint, time-to-update.
