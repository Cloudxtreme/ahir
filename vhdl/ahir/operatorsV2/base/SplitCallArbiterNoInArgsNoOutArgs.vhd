library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library ahir;
use ahir.Types.all;
use ahir.Subprograms.all;
use ahir.Utilities.all;
use ahir.BaseComponents.all;

entity SplitCallArbiterNoInArgsNoOutArgs is
  generic(num_reqs: integer;
	  caller_tag_length: integer;
          callee_tag_length: integer);
  port ( -- ready/ready handshake on all ports
    -- ports for the caller
    call_reqs   : in  std_logic_vector(num_reqs-1 downto 0);
    call_acks   : out std_logic_vector(num_reqs-1 downto 0);
    call_tag    : in  std_logic_vector((num_reqs*caller_tag_length)-1 downto 0);
    -- call port connected to the called module
    call_mreq   : out std_logic;
    call_mack   : in  std_logic;
    call_mtag   : out std_logic_vector(callee_tag_length-1 downto 0);
    -- similarly for return, initiated by the caller
    return_reqs : in  std_logic_vector(num_reqs-1 downto 0);
    return_acks : out std_logic_vector(num_reqs-1 downto 0);
    return_tag  : out std_logic_vector((num_reqs*caller_tag_length)-1 downto 0);
    -- return from function
    return_mreq : out std_logic;
    return_mack : in std_logic;
    return_mtag : in  std_logic_vector(callee_tag_length-1 downto 0);
    clk: in std_logic;
    reset: in std_logic);
end SplitCallArbiterNoInArgsNoOutArgs;


architecture Struct of SplitCallArbiterNoInArgsNoOutArgs is
   signal pe_call_reqs: std_logic_vector(num_reqs-1 downto 0);
   signal return_acks_sig: std_logic_vector(num_reqs-1 downto 0);

   type TagwordArray is array (natural range <>) of std_logic_vector(caller_tag_length-1 downto 0);
   signal return_tag_sig : TagwordArray(num_reqs-1 downto 0);

   type CallStateType is (idle, busy);
   signal call_state: CallStateType;

   signal latch_call_data : std_logic;
   signal call_mtag_prereg  : std_logic_vector(callee_tag_length-1 downto 0);
begin

  -----------------------------------------------------------------------------
  -- priority encode incoming
  -----------------------------------------------------------------------------
   pe_call_reqs <= PriorityEncode(call_reqs);

   ----------------------------------------------------------------------------
   -- process to handle call_reqs  --> call_mreq muxing
   ----------------------------------------------------------------------------
   process(clk,pe_call_reqs,call_state)
        variable nstate: CallStateType;
        variable there_is_a_call : std_logic;
   begin
	nstate := call_state;
        there_is_a_call := OrReduce(pe_call_reqs);
	latch_call_data <= '0';
	call_mreq <= '0';

	if(call_state = idle) then
		if(there_is_a_call = '1') then
			latch_call_data <=  '1';
			nstate := busy;
		end if;
	elsif (call_state = busy) then
		call_mreq <= '1';
		if(call_mack = '1') then
			nstate := idle;
		end if;
	end if;
	
	if(clk'event and clk = '1') then
		if(reset = '1') then
			call_state <= idle;
		else
			call_state <= nstate;
		end if;
	end if;
   end process;


   -- combinational process.. generate call_acks, and also
   -- mux to input of call data register.
   process(pe_call_reqs,latch_call_data)
   begin
	call_acks <= (others => '0');
       	for I in num_reqs-1 downto 0 loop
       		if(pe_call_reqs(I) = '1') then
			if(latch_call_data = '1') then
				call_acks(I) <= '1';
			end if;
       		end if;
	end loop;
   end process;

   -- call data register.
   process(clk)
   begin
     if(clk'event and clk = '1') then
     	if(latch_call_data = '1') then
		call_mtag <= call_mtag_prereg;
        end if;  -- I
     end if;
   end process;
 
   -- tag generation.
   tagGen : BinaryEncoder generic map (iwidth => num_reqs,
                                       owidth => callee_tag_length)
     port map (din => pe_call_reqs, dout => call_mtag_prereg);

   -- on a successful call, register the tag from the caller
   -- side..
   tagRegGen: for T in 0 to num_reqs-1 generate
     process(clk)
     begin
       if(clk'event and clk = '1') then
         if(pe_call_reqs(T) = '1' and latch_call_data = '1') then
           return_tag_sig(T)
             <= call_tag(((T+1)*caller_tag_length)-1 downto T*caller_tag_length);
         end if;
       end if;
     end process;     
   end generate tagRegGen;

   ----------------------------------------------------------------------------
   -- reverse path
   ----------------------------------------------------------------------------
   process(return_tag_sig)
     variable lreturn_tag : std_logic_vector((num_reqs*caller_tag_length)-1 downto 0);
   begin
     for J in return_tag_sig'high(1) downto return_tag_sig'low(1) loop
       Insert(lreturn_tag,J,return_tag_sig(J));
     end loop;  -- J
     return_tag <= lreturn_tag;
   end process;

   -- always ready to accept return data..
   return_mreq <= '1';

   -- the acks in both directions
   return_acks <= return_acks_sig;
   
   -- incoming data written into appropriate register.
   RetGen: for I in return_reqs'high downto return_reqs'low generate

     fsm: block
       signal ack_reg,  valid_flag : std_logic;
     begin  -- block fsm

       -- valid = '1' implies this index is incoming
       valid_flag <= '1' when return_mack = '1' and (I = To_Integer(To_Unsigned(return_mtag))) else '0';

       --------------------------------------------------------------------------
       -- ack ff
       --------------------------------------------------------------------------
       -- set if valid_flag is asserted, else clear if return_reqs is asserted
       -- and register is already set.
       process(clk)
       begin
         if clk'event and clk= '1' then
           if(reset = '1') then
             ack_reg <= '0';
           elsif valid_flag = '1' then
             ack_reg <= '1';
           elsif return_reqs(I) = '1' and ack_reg = '1' then
             ack_reg <= '0';
           end if;

         end if;
       end process;

       -- pass info out of the generate
       return_acks_sig(I) <= ack_reg;
       
     end block fsm;

     
   end generate RetGen;
end Struct;
