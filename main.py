import sys
from simso.core import Model
from simso.configuration import Configuration
from interval import *
from ss_conf_table import *
#from ss_offline_phase import gen_slotshift_table

def main():

	remote = reciever("/tmp/ss_parser")

	configuration = Configuration()

	if remote == None:
		print "remote is none"
		#todo: this number should be generated randomly
		table = gen_slotshift_table(1, 10, 10, 10, .10)
		if table:
			remote = local_table(table[0], table[1],
				table[2], table[3])
		else:
			print "Cant Run because task set not generated"
			return
	else:
		remote.recieve_table()

	associate = association(remote)

	interval = associate.create_def_intr_list(associate)
	
	run_time = associate.get_running_time(interval)

	associate.create_relation_window(associate, interval)

	while 1:
		j = associate.create_tsk(associate, interval, configuration)
		if(j != 0):
			break

	configuration.duration = run_time * configuration.cycles_per_ms

	configuration.add_processor(name="CPU 1", identifier=1)

	configuration.scheduler_info.filename = "./SlotShifting.py"

	configuration.scheduler_info.data = interval

	configuration.check_all()

	model = Model(configuration)

	model.run_model()


main()	
