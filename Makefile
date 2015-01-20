.PHONY: clean All

All:
	@echo "----------Building project:[ pgen - Test ]----------"
	@$(MAKE) -f  "pgen.mk"
clean:
	@echo "----------Cleaning project:[ pgen - Test ]----------"
	@$(MAKE) -f  "pgen.mk" clean
