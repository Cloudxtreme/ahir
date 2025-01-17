library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_textio.all;

library ahir;
use ahir.types.all;
use ahir.subprograms.all;
use ahir.LoadStorePack.all;

library std;
use std.textio.all;

entity testbench is

end testbench;

architecture default_arch of testbench is

  component system
    port (
      clk             : in  std_logic;
      env_call_ack    : out std_logic;
      env_call_data   : in  std_logic_vector(0 downto 0);
      env_call_req    : in  std_logic;
      env_call_tag    : in  std_logic_vector(3 downto 0);
      env_lc_ack      : out std_logic;
      env_lc_data     : out std_logic_vector(7 downto 0);
      env_lc_req      : in  std_logic;
      env_lc_tag      : out std_logic_vector(7 downto 0);
      env_lr_ack      : out std_logic;
      env_lr_addr     : in  std_logic_vector(15 downto 0);
      env_lr_req      : in  std_logic;
      env_lr_tag      : in  std_logic_vector(7 downto 0);
      env_return_ack  : in  std_logic;
      env_return_data : out std_logic_vector(16 downto 0);
      env_return_req  : out std_logic;
      env_return_tag  : out std_logic_vector(3 downto 0);
      env_sr_ack      : out std_logic;
      env_sr_addr     : in  std_logic_vector(15 downto 0);
      env_sr_data     : in  std_logic_vector(7 downto 0);
      env_sr_req      : in  std_logic;
      env_sr_tag      : in  std_logic_vector(7 downto 0);
      reset           : in  std_logic); 
  end component;
  
  signal env_call_ack    : std_logic;
  signal env_call_data   : std_logic_vector(0 downto 0) := (others => '0');
  signal env_call_tag   : std_logic_vector(3 downto 0) := (others => '0');
  signal env_call_req    : std_logic := '0';
  signal env_return_ack  : std_logic := '0';
  signal env_return_data : std_logic_vector(16 downto 0);
  signal env_return_tag : std_logic_vector(3 downto 0);
  signal env_return_req  : std_logic;

  signal clk             : std_logic := '0';
  signal env_lc_ack      : std_logic;
  signal env_lc_data     : std_logic_vector(7 downto 0);
  signal env_lc_tag      : std_logic_vector(7 downto 0);
  signal env_lc_req      : std_logic                    := '0';
  signal env_lr_ack      : std_logic;
  signal env_lr_addr     : std_logic_vector(15 downto 0) := (others => '0');
  signal env_lr_tag      : std_logic_vector(7 downto 0) := (others => '0');
  signal env_lr_req      : std_logic                    := '0';
  signal env_sr_ack      : std_logic;
  signal env_sr_addr     : std_logic_vector(15 downto 0) := (others => '0');
  signal env_sr_data     : std_logic_vector(7 downto 0) := (others => '0');
  signal env_sr_tag      : std_logic_vector(7 downto 0) := (others => '0');
  signal env_sr_req      : std_logic                    := '0';
  signal reset           : std_logic                    := '1';

  file memory_init : text;

begin  -- default_arch

  clk <= not clk after 10 ns;

  process
  begin  -- process
    wait on clk until clk = '1';
    reset <= '0';
    wait;
  end process;

  process

    variable ok : file_open_status;
    variable l : line;
    variable init_addr : natural;
    variable init_data : std_logic_vector(lau - 1 downto 0);

  begin  -- process

    wait until reset = '0';

    file_open(ok, memory_init, "memory_init.txt", read_mode);
    assert ok = open_ok report "unable to open the init file" severity failure;
    
    env_sr_req <= '1';
    
    init_mem_loop: while not endfile(memory_init) loop

      readline(memory_init, l);
      next init_mem_loop when l'length = 0;
      read(l, init_addr);
      read(l, init_data);
      
      env_sr_addr <= std_logic_vector(to_unsigned(init_addr, maw));
      env_sr_data <= init_data;

      while true loop
        wait until clk = '1';
        if env_sr_ack = '1' then
          exit;
        end if;
      end loop;
      
    end loop;
    
    env_sr_req <= '0';
    
    wait until clk = '1';
    env_call_req <= '1';
    wait until clk = '1';
    while env_call_ack = '0' loop
      wait until clk = '1';
    end loop;
    env_call_req <= '0';

    env_return_ack <= '1';
    wait until clk = '1';
    while env_return_req = '0' loop
      wait until clk = '1';
    end loop;
    env_return_ack <= '0';

    assert false report "simulation done" severity note;
    wait;
  end process;

  system_1: system
    port map (
      clk             => clk,
      env_call_ack    => env_call_ack,
      env_call_data   => env_call_data,
      env_call_req    => env_call_req,
      env_call_tag    => env_call_tag,
      env_lc_ack      => env_lc_ack,
      env_lc_data     => env_lc_data,
      env_lc_req      => env_lc_req,
      env_lc_tag      => env_lc_tag,
      env_lr_ack      => env_lr_ack,
      env_lr_addr     => env_lr_addr,
      env_lr_req      => env_lr_req,
      env_lr_tag      => env_lr_tag,
      env_return_ack  => env_return_ack,
      env_return_data => env_return_data,
      env_return_req  => env_return_req,
      env_return_tag  => env_return_tag,
      env_sr_ack      => env_sr_ack,
      env_sr_addr     => env_sr_addr,
      env_sr_data     => env_sr_data,
      env_sr_req      => env_sr_req,
      env_sr_tag      => env_sr_tag,
      reset           => reset); 

end default_arch;
