import sys
from simso.core import Model
from simso.configuration import Configuration
from interval import *
from ss_conf_table import *


def main():
	
	remote = reciever("/tmp/ss_parser")
		
	configuration = Configuration()

	remote.recieve_table()
	
	associate = association(remote)

	interval = associate.create_intr_list(associate)

	associate.create_relation_window(associate, interval)
	
	while 1:
		j = associate.create_tsk(associate, interval, configuration)
		if(j != 0):
			break

	configuration.duration = 420 * configuration.cycles_per_ms
	
	configuration.add_processor(name="CPU 1", identifier=1)

	configuration.scheduler_info.filename = "./SlotShifting.py"

	configuration.scheduler_info.data = interval

	

main()	
