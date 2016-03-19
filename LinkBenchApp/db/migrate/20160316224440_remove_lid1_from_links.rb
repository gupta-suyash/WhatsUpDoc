class RemoveLid1FromLinks < ActiveRecord::Migration
  def change
	remove_column :links, :id1, :integer
  end
end
